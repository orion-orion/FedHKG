import argparse
import os
from openke.data import TrainDataLoader, ValidDataLoader, TestDataLoader
import numpy as np
from pprint import pprint
import pickle
import random
from generate_data import read_data, split_data


def arg_parse():
    parser = argparse.ArgumentParser()

    # dataset part
    parser.add_argument('--dataset', type=str, default='FB13',
                        help='dataset, possible are `FB13`, `FB15K`,``FB15K237`, `NELL-995`, `WN11`, \
                            `WN18`, `WN18RR`, `YAGO3-10`')
    parser.add_argument('--n_clients', type=int,
                        default=10, help="number of clients")
    parser.add_argument(
        '--train_frac', help='fraction of train samples', type=float, default=0.8)
    parser.add_argument(
        '--valid_frac', help='fraction of validation samples in train samples', type=float, default=0.2)

    # knowledge graph part
    parser.add_argument('--n_batches', type=int, default=100,
                        help="number of batches of the knowledge graph dataset to use")
    parser.add_argument('--n_threads', type=int, default=8,
                        help="number of threads to load the knowledge graph")
    parser.add_argument('--sampling_mode', type=str, default='normal',
                        help='knowledge graph sampling mode, possible are `normal`, `link`')
    parser.add_argument('--neg_ent', type=int, default=1,
                        help="number of negative entities for each triple. Note that it must >=1 to make sure that training dataset includes all entities")
    parser.add_argument('--neg_rel', type=int, default=0)
    parser.add_argument('--bern_flag', type=bool, default=True,
                        help='bern flag, possible are True, False')
    parser.add_argument('--filter_flag', type=bool, default=True,
                        help='filter flag, possible are True, False')

    # random seed
    parser.add_argument('--seed', type=int, default=42, help='random seed')

    args = parser.parse_args()
    return args


def seed_everything(args):
    random.seed(args.seed)
    np.random.seed(args.seed)
    os.environ['PYTHONHASHSEED'] = str(args.seed)


if __name__ == "__main__":
    args = arg_parse()

    seed_everything(args)

    in_file_path = "./benchmarks/" + args.dataset + "/"
    # dataloader for training
    train_dataloader = TrainDataLoader(
        in_path=in_file_path,
        nbatches=args.n_batches,
        threads=args.n_threads,
        sampling_mode=args.sampling_mode,
        bern_flag=args.bern_flag,
        filter_flag=args.filter_flag,
        neg_ent=args.neg_ent,
        neg_rel=args.neg_rel
    )
    valid_dataloader = ValidDataLoader(in_file_path, "normal")
    test_dataloader = TestDataLoader(in_file_path, "normal")

    triples = read_data(train_dataloader, valid_dataloader, test_dataloader)
    client_data = split_data(triples, args)

    with open(os.path.join("data", args.dataset+"-Fed%d.pkl" % args.n_clients), "wb") as f:
        pickle.dump(client_data, f)
