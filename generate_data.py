import numpy as np
import random
import copy


def read_data(train_data_loader, valid_data_loader, test_data_loader):
    triples = []
    for data in train_data_loader:
        # data:{"batch_h": (22568,), "batch_t":(22568,),"batch_r",(22568,), "batch_y":(22568,)}
        # batch_y: [1.,1.,...,-1.,-1.]标注正负样本
        # 不过batch_y在forward过程中(一般forward输出是loss)没啥用
        triples.extend(list(np.stack(
            [data["batch_h"].T, data["batch_t"].T, data["batch_r"].T], axis=1)))
    for data in valid_data_loader:
        data, _ = data
        triples.extend(list((np.stack(
            [data["batch_h"].T, data["batch_t"].T, data["batch_r"].T], axis=1))))
    for data in test_data_loader:
        data, _ = data
        triples.extend(list((np.stack(
            [data["batch_h"].T, data["batch_t"].T, data["batch_r"].T], axis=1))))
    return triples


def split_relation(triples, n_clients):
    random.shuffle(triples)
    triples = np.stack(triples, axis=0)
    edge_types = list(set(triples[:, 2]))
    edge_type_to_cid = {}
    n_edge_types_per_client = len(edge_types)//n_clients
    for id, edge_type in enumerate(edge_types):
        c_id = id // n_edge_types_per_client
        if c_id < n_clients - 1:
            edge_type_to_cid[edge_type] = c_id
        else:
            edge_type_to_cid[edge_type] = n_clients - 1
    return edge_type_to_cid


def split_triples(triples, edge_type_to_cid, n_clients):
    c_id_triples = [[] for i in range(n_clients)]
    for triple in triples:
        edge_type = triple[2]
        c_id = edge_type_to_cid[edge_type]
        c_id_triples[c_id].append(triple.reshape(1, -1))
    for c_id, triples in enumerate(c_id_triples):
        c_id_triples[c_id] = np.concatenate(triples)
    return c_id_triples


def indices_mapping(c_id_triples_ori, c_id_triples, n_clients):
    """mapping global indices to local indices"""
    for c_id, triple_ori in enumerate(c_id_triples_ori):
        edge_index = triple_ori[:, :2]
        edge_type = triple_ori[:, 2]

        # map entity indices to local entity indices
        index_mapping = {}
        entities = list(set(edge_index.flatten()))
        for index, entity in enumerate(entities):
            index_mapping[entity] = index

        def f(x): return index_mapping[x]
        f = np.vectorize(f)
        client_entity_local_index = f(edge_index)

        # map edge indices to local entity indices
        index_mapping = {}
        edges = copy.deepcopy(list(set((edge_type))))
        for index, edge in enumerate(edges):
            index_mapping[edge] = index

        def f(x): return index_mapping[x]
        f = np.vectorize(f)
        client_edge_local_index = f(edge_type)

        c_id_triples[c_id] = np.concatenate([client_entity_local_index,
                                             client_edge_local_index.reshape(-1, 1)], axis=1)


def split_data(triples, args):
    client_data = [{"train": {}, "valid": {}, "test": {}}
                   for i in range(args.n_clients)]

    edge_type_to_cid = split_relation(triples, args.n_clients)

    cid_triples = split_triples(triples, edge_type_to_cid, args.n_clients)

    cid_triples_ori = copy.deepcopy(cid_triples)
    indices_mapping(cid_triples_ori, cid_triples, args.n_clients)

    # split train, valid, test dataset
    for c_id in range(args.n_clients):
        n_triples = cid_triples[c_id].shape[0]
        n_train = int(n_triples * args.train_frac)
        n_test = n_triples - n_train
        n_val = int(n_train * args.valid_frac)
        n_train = n_train - n_val
        mod_to_slice = {"train": slice(0, n_train),
                        "valid": slice(n_train, n_train+n_val), "test": slice(-n_test, n_triples)}
        for mode in ["train", "valid", "test"]:
            client_data[c_id][mode]["edge_index_ori"] = cid_triples_ori[c_id][mod_to_slice[mode], : 2].T
            client_data[c_id][mode]["edge_index"] = cid_triples[c_id][mod_to_slice[mode], : 2].T

            client_data[c_id][mode]["edge_type_ori"] = cid_triples_ori[c_id][mod_to_slice[mode], 2]
            client_data[c_id][mode]["edge_type"] = cid_triples[c_id][mod_to_slice[mode], 2]

    return client_data
