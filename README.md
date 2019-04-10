# Head3D
一款与结构光三维扫描仪结合使用的人体头部三维测量软件

1、使用软件前需在本地配置Mysql数据库，打开my.ini更改basedir和datadir为本地路径；
2、编辑系统环境变量，在path变量中添加Mysql的bin目录，重启电脑；
3、在cmd中:d进入Mysql的bin目录，输入mysqld --install进行安装；
4、安装完成后继续输入mysqld  --initialize进行初始化；
5、最后输入net start mysql启动Mysql服务；
6、打开Mysql后创建Head3D数据库；
7、打开Head3D.exe即可运行。

注：
*若Mysql安装出错，请安装VC运行库之后再进行尝试
*若Mysql初始化出错，请注意my.ini配置文件编码格式
*扫描保存的模型存储路径不能包含中文和空格符
