<!--
 * @Descripttion: 
 * @Version: 1.0
 * @Author: ZhangHongYu
 * @Date: 2022-07-04 17:31:00
 * @LastEditors: ZhangHongYu
 * @LastEditTime: 2022-07-07 15:14:04
-->
<!--
 * @Descripttion: 
 * @Version: 1.0
 * @Author: ZhangHongYu
 * @Date: 2022-07-03 20:27:59
 * @LastEditors: ZhangHongYu
 * @LastEditTime: 2022-07-04 17:04:43
-->
<p align="center">
<img src="pic/logo.png" width="600" height="200">
</p>

<div align="center">

# FedHKG: 联邦异构知识图谱划分工具箱

[![Open Source Love](https://badges.frapsoft.com/os/v2/open-source.svg?v=103)](https://github.com/orion-orion/FedHKG)[![](https://img.shields.io/github/license/orion-orion/FedHKG)](https://github.com/orion-orion/FedHKG/blob/main/LICENSE)[![](https://img.shields.io/github/stars/orion-orion/FedHKG?style=social)](https://github.com/orion-orion/FedHKG)
<br/>
[![](https://img.shields.io/github/directory-file-count/orion-orion/FedHKG)](https://github.com/orion-orion/FedHKG) [![](https://img.shields.io/github/languages/code-size/orion-orion/FedHKG)](https://github.com/orion-orion/FedHKG) 
</div>

## 1 简介
[FedHKG](https://github.com/orion-orion/FedHKG.git)为联邦异构知识图谱划分的工具箱，旨在提供为联邦场景下的知识图谱提供Non-IID的划分算法实现。在联邦场景下， $C$ 个知识图谱 $\{\mathcal{E}_c, \mathcal{R}_c, \mathcal{T}_c\}$ 位于 $C$ 个不同的客户端上。再常见的银行风控、医疗知识图谱场景下，知识图谱拥的实体集合 $\mathcal{E}_c$ 之间可能会存在重叠，而其关系集合 $\mathcal{R}_c$ 和元组集合 $\mathcal{T}_c$ 之间则不会重叠<sup>[1]</sup>。比如在不同客户端对应不同银行的情况下，由于不同银行都有着自己的业务流程，所以关系集合不重叠。

由于我们这里的本地知识图谱 $\{\mathcal{E}_c, \mathcal{R}_c, \mathcal{T}_c\}$ 中每个知识图谱的关系 $\mathcal{R}_c$ （即边的种类）是不同的，我们在划分元组之前我们需要先对关系进行划分，然后针对关系划分的结果来划分元组。 待元组划分到本地后，还需要将原有的实体和关系的索引映射到本地索引。最后，再在本地进行训练/验证/测试集的拆分。整体数据划分流程图如下：

<p align="center">
<img src="https://images.cnblogs.com/cnblogs_com/blogs/538207/galleries/2108041/o_221026095716_%E8%81%94%E9%82%A6%E7%9F%A5%E8%AF%86%E5%9B%BE%E8%B0%B1%E6%95%B0%E6%8D%AE%E5%88%92%E5%88%86%E6%B5%81%E7%A8%8B%E5%9B%BE.png" width = "680" height="120">
</p>

其中在**确定关系划分**部分，我们选择随机地将关系 $\mathcal{R}$ 不重叠地划分到不同的client上；在关系的划分确定之后，我们进入**划分元组**部分，此时可以根据每个元组 $(h,r,t)$ 中 $r$ 的划分情况来决定该元组的划分情况。划分好元组之后，子图就确定了，接下来就是**索引映射**部分：即将子图的实体和关系的索引进行重新编号，如下图所示：

<p align="center">
<img src="https://images.cnblogs.com/cnblogs_com/blogs/538207/galleries/2108041/o_bdba7568.png" width = "670" height="400">
</p>

对于具体的局部索引如何安排，我们采用随机选择的方式。最后，进入**训练、验证、测试集拆分**部分，即还需要在本地划分训练集、验证集和测试集。数据集划分完毕之后，则训练/验证/测试集对应的实体(edge_index)和关系类型(edge_type)就都确立了。

## 2 环境依赖
我的Python版本为`3.9.13 `，numpy版本为`1.24.2`

## 3 数据集

数据集我已经将其上传到了Google drive，大家可自行下载并放在项目目录中的`benchmarks`目录下。下载链接可参见：

[Google drive 下载链接](https://drive.google.com/drive/folders/1l4BbSaxDv5Nswe0A1MZo5xRUB7cR6orM?usp=sharing)


## 4 使用方法
### 4.1 编译openKG模块

您可以先`cd openke`进入到`openke`目录，然后在运行`make.sh`编译脚本
```bash
cd openke
bash make.sh x86
```
注意，你如果是`x86`架构（大部分Linux服务器）请传入`x86`参数。如果你是M1芯片的Mac，请将参数修改为`apple-m1`。

如果编译成功，你会看到生成`openke/release`目录，且该目录下有个`Base.so`动态运行库文件。

### 4.2 生成联邦异构知识图谱数据集

之后，再运行`main.py`来生成联邦异构知识图谱数据集：
```bash
python main.py \
        --dataset FB13 \
        --n_clients 10 \
        --train_frac 0.8 \
        --valid_frac 0.2 \
        --seed 42
```
这里`--dataset`参数用于指定知识图谱数据集，可选的包括`FB13`、`FB15K`、`FB15K237`、`NELL-995`、`WN11`、`WN18`、`WN18RR`、`YAGO3-10`；`--n_clients`用于指定客户端的个数；`--train_frac`用于指定训练集所占的比例，`--valid_frac`用于指定在训练集中划分出验证集的比例。`--seed`为随机数种子。

数据集划分算法完毕后，可以在`data`目录下看到所乘车的联邦异构数据集文件。比如`FB13-Fed10.pkl`文件就是在`FB13`数据集和10个客户端的设置下所生成的联邦异构数据集。你可以使用`pickle`模块对其对其进行加载，并查看其一些统计量信息：
```python
In [1]: import pickle

In [2]: import collections

In [3]: all_data = pickle.load(open('data/FB13-Fed10.pkl', 'rb'))

In [4]: all_data[0]['train']['edge_type'].shape
Out[4]: (18500,)

In [5]: all_data[0]['train']['edge_type_ori'].shape
Out[5]: (18500,)

In [6]: dict_ = dict(collections.Counter(all_data[0]['train']['edge_type']))

In [7]: dict_origin = dict(collections.Counter(all_data[0]['train']['edge_type_ori']))

In [8]: sorted(list(dict_))
Out[8]: [0]

In [9]: sorted(list(dict_origin))
Out[9]: [4]
```


## 参考
[1] Chen M, Zhang W, Yuan Z, et al. Fede: Embedding knowledge graphs in federated setting[C]//The 10th International Joint Conference on Knowledge Graphs. 2021: 80-88.