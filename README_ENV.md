# JND-based Perceptual Rate Distortion Optimization for AV1 Encoder (PCS2019)

## 环境配置
### 代码的基础环境是aom v1.0.0, vs2019, matlab2019b，其余环境和build流程请参考[官方说明](https://aomedia.googlesource.com/aom/)和[相关资料](https://blog.csdn.net/Annie_heyeqq/article/details/126728967)：
* [CMake](https://cmake.org/)
* [Git](https://git-scm.com/)
* [Perl](https://www.perl.org/)
* [yasm](http://yasm.tortall.net/)
* [doxygen](https://www.doxygen.nl/)

build完成后，将其中的aomenc设置为启动项目。

### JND模型的dll库生成
利用matlab的mcc编译器生成C代码和dll库，[流程](https://zhuanlan.zhihu.com/p/507321132)如下：
* 打开matlab，切换至jnd目录，在命令窗口中输入：
```
mex -setup
mbuild -setup
mcc -W lib:jnd_mat -T link:lib func_JND_modeling_pattern_complexity.m
```
* 将产生的jnd_mat.h和jnd_mat.c移动至av1/encoder目录，jnd_mat.lib移动至build目录，jnd_mat.dll移动至.exe可执行文件目录
* 配置VS包含目录：选择项目aomenc和aom_av1_encoder属性的'VC++目录'选项卡，在'包含目录'中添加matlab头文件路径：“matlab安装路径”\extern\include
* 配置VS库目录：选择项目aomenc属性的'VC++目录'选项卡，在'库目录'中添加matlab静态链接库路径：“matlab安装路径”\extern\lib\win64\microsoft
* 配置链接器：项目aomenc属性的链接器->输入：mclmcrrt.lib; mclmcr.lib; libmat.lib; libmx.lib

注意：配置目录时，属性页选择'所有配置'

## 常见问题
* [运行代码显示'帧不在模块中'](https://blog.csdn.net/lvh98/article/details/124754645)：调试–>窗口–>异常设置，取消勾选Win32 Exceptions
* 修改JND因子的加权方式：1. jnd目录.m文件；2. av1/encoder/jnd.h中的// jnd factor weigh相关部分