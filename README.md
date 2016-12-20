## QT-SQLiteStudio

### 注意事项：
* 本工具对sqlite数据库加密用的是网上开源库，请在编译此工具前先编译此库，并把dll文件与代码链接上，以下是SQLITECIPHER开源库：
![SQLITECIPHER开源库](https://github.com/cheng668/QtCipherSqlitePlugin)

* 本工具暂时运用了openssl的des算法保存密码，所以要求你的电脑有安装openssl并配置好环境变量

**[OPENSSL下载](http://www.openssl.com/download.html)**

**[OPENSSL环境变量配置](http://cheng668.com/2016/12/14/OPENSSL-Develop-Config/)**

* 工具通过/Resources/qss/mainwindow.qss设置样式，个人可以根据个人审美对样式表进行修改

### SQLiteStudio用途：
* 可以根据.sql文件创建.db文件，并实现加密
* 查看和修改db文件
* 学习QT

### SQLiteStudio用法：

#### 查看DB和创建DB页通用属性：
* 右上角的密码输入框能输入最多32位非空字符，用于打开数据库。如果为不输入或者为空，则只能打开未加密的.db文件，在关闭工具时会把密码通过DES算法加密存进配置文件中。
* 选择下拉框最后一项![](https://raw.githubusercontent.com/cheng668/QT-SQLiteStudio/master/Resources/images/clear.png)可清楚下拉框，可打开的文件能存进配置文件，方便再次打开工具时加载进来。
* ![](https://raw.githubusercontent.com/cheng668/QT-SQLiteStudio/master/Resources/images/folder.png)按钮用于打开.sql或者.db文件，该按钮右边的按钮用于关闭当前数据库

#### 查看DB
##### 用户查看.db文件，并可用于修改.db文件内容
* 右下的其他按钮用于对.db文件内容进行操作
* 左边文本框显示的是当前打开的.db的表项

#### 创建DB
##### 用户根据.sql文件创建.db文件，并可保存已修改的.sql文件创建
* 执行按钮用于执行.sql向.db文件的转换
* 分隔符是用于在.sql向.db文件转换时标识一句SQL脚本，如果没有分隔符的话，此软件很难识别出SQL语句，如果分隔符是$，则.sql文件中语句应该这样写：

```
$/* 这行是注释写法，下面是删除表的SQL语句写法 */
```
```
$drop table if exists table1;
```
* 最下面一行是执行转换时的进度条

### 最后
* 欢迎对此SQLiteStudio工具进行修改，让它更完美！！
