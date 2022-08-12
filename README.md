# FasterChantDevice
《300高速咏唱装置》一个外部实现的简易一键喊话软件  [【下载地址】](https://github.com/Anran-233/FasterChantDevice/releases)

![image](https://user-images.githubusercontent.com/111073265/184226513-2c329eae-fcf2-4e29-b9ba-57967952aaa5.png)

# 主要实现原理
原理其实非常简单，就是用了一个键盘钩子来监听，触发条件后呼出一个完全前置但设置了焦点穿透的半透明显示窗口。
然后继续监听键盘事件，对相应的按键做出处理，并同步调整显示面板上的内容就行了。
至于发送信息，是使用了虚拟键盘模拟，先将要发送的文字放到剪贴板里，然后模拟虚拟按键：【回车】(打开聊天框)->【Ctrl】+【V】(粘贴)->【回车】(发送消息)。

# 操作说明
### 如何呼出面板
- 根据设置上的【启动按键】，按下对应的按键即可呼出显示面板。
  - 【显示位置】会决定显示面板在左侧还是右侧，面板的高度会对齐你电脑上当前焦点窗口的高度；
  - 【仅限三百】启用时，只会在 **300英雄** 中呼出显示面板；
  - 【全体发言】启用时，会在【回车】打开聊天框时，先模拟按住【shift】，这样会打开全体频道进行发言；
  - *注意：当前选中方案为空时，是无法呼出显示面板的*；
  - *注意：部分电脑因为权限问题，也会出现无法呼出面板的情况，使用管理员权限启动软件即可*；

![image](https://user-images.githubusercontent.com/111073265/184236803-ed50a09e-870b-4141-bc32-19b7a6a20d31.png)

### 如何编辑和发送 快捷发言

#### 【二级面板】不启用时
- 主要编辑界面，每个预设方案可以至多编辑9条快捷发言。
- 具体操作：【启动热键】(呼出显示面板)+【数字键】(选中对应快捷发言直接发送)。

#### 【二级面板】启用时
- 主要编辑界面的9条快捷发言失去原本的发送意义，而被当做二级面板的索引使用。
- 点击右边【二级】按键，可以编辑对应索引下的二级面板，每个二级面板可以编辑至多10条快捷发言。
- 具体操作：【启动热键】(呼出显示面板)+【数字键】(选中索引，进入对应索引的二级面板)+【数字键】(选中二级面板中的对应快捷发言并直接发送)。
- 由原本的两次按键发送，变成了三次按键发送，多了一次按键选择的过程，同时也由原本的9条快捷发言，增加到了90条快捷发言可供选择。
- *是否开启二级面板各有优劣，大家可以根据情况自行选择。*
  
![image](https://user-images.githubusercontent.com/111073265/184236960-d50b62e9-0d2b-4363-8340-6c451e733b97.png)

### 切换/搜索 预设方案

#### 在主界面时
  直接点击左侧【选中】按钮，来切换当前预设方案。
  
![image](https://user-images.githubusercontent.com/111073265/184240845-a213dd13-17da-4802-9ca6-9eda400e229d.png)

#### 在使用呼出的显示面板过程中
- 在显示面板的首页，`1`~`9` 数字键是选择快捷发言，而`0`数字键就是留作切换方案的特殊按键；
- 按`0`键可进入搜索预设方案界面，此时有两种搜索方案，会根据接下来的按键进入不同的搜索方式：
  - 按【数字键】时，通过方案编号来切换方案，因为最多99个预设方案，所以只需再按一次【数字键】(一共2次)，即可切换到其他预设方案(前提是该方案存在)(面板中会筛选出十位数是第一次按下数字的预设方案列表)。
  - 按【字母键】时，通过方案名称的首字母缩写来筛选方案(无视方案名称里的非中文和非英文字符)，筛选结果会显示在面板列表中(按0~9编号)，继续按【字母键】可以缩写筛选范围，如果此时按下【数字键】，会选中列表中相应的预设方案(前提是该方案存在)，然后切换并回到面板首页。
  - *退格可以回退按键。*

![image](https://user-images.githubusercontent.com/111073265/184244057-edd61a06-fa82-4390-8cde-e289d9b3600e.png)
![image](https://user-images.githubusercontent.com/111073265/184244553-ca3fdadd-d021-4d6f-8871-009e1be547b9.png)

![image](https://user-images.githubusercontent.com/111073265/184244474-f25083d2-7667-4e1e-b9f4-542a47e5bbcf.png)
![image](https://user-images.githubusercontent.com/111073265/184244409-d7c31707-d2a9-4816-a8d7-19b0aa32640c.png)

### 自定义显示面板的样式
  做的比较简陋，并没有太多精力去设计一个好看的UI，但考虑到不同场景下的对比度显示问题，干脆把调整色彩样式这些元素弄给你们自己去设置了，单击右上角的【样式设置】，就可以打开对应的设置面板调整样式。
  
![image](https://user-images.githubusercontent.com/111073265/184245615-16e95731-a5b3-44ae-b6a8-cc66a581e759.png)

# 高级操作(非必须知道)
  *注意：此节内容需要一定理解能力。*

### 入门级：在其他游戏使用
- 关掉【仅限三百】，你几乎可以在其他任何支持聊天的游戏中使用(例如LOL)，只要这些游戏的聊天操作是这样的：【回车】->【打字】->【回车】，那么你就可以使用。
- 在没有全体频道的游戏中，你需要关掉【全体发言】，这样发送的时候不会因为按住【shift】键而出问题。

#### *本节核心操作：点击右上角【高级设置】按钮，将普通设置面板切换到高级设置面板！*
![image](https://user-images.githubusercontent.com/111073265/184247061-afcac79a-5b04-40a0-a59c-898bc2a9db48.png)

### 直接在聊天软件或者任意快捷打字场景中使用
- 将【游戏模式】切换为【聊天模式】，并启用热键屏蔽输出，你就可以在任意打字场景下使用软件的快捷发言(例如QQ、微博评论、论坛留言等)。

![image](https://user-images.githubusercontent.com/111073265/184248289-82b55b7f-27bf-4fe7-bd34-caa21e7180f4.png)
![image](https://user-images.githubusercontent.com/111073265/184248395-d96f9259-db69-4ff5-9845-c50f20af8d87.png)


### 连发模式
- 将【关闭连发】切换到具体的间隔时间，就可以开启【连发模式】：
  - 【连发模式】下，二级面板的开关无论 开启/关闭 都会失效，主编辑面板的9条快捷发言依旧是作为索引使用。
  - 具体操作时不需要按第二次【数字键】来选择二级面板中的快捷发言，因为二级面板中的所有发言，会按照从上往下的顺序依次发送。每条发送之间的间隔时间，就是之前【连发模式】里设置的间隔时间。(例如【间隔：1s】，就是每隔一秒发送一次，直到二级面板中的内容发送完，遇到空的内容会跳过)
- 【连发模式】上一个连续发言还没有发送完时，无法开启下一段连续发言，在面板首页按`0`键进入切换方案面板，就可以终止上一个连续发言。
- 在面板首页按【空格】不放的同时，按下`1`~`9`数字键，可以快速设置【连发模式】间隔1-9秒，而按下`0`数字键是关闭连发模式。
- 【连发模式】还可以配合上面的【聊天模式】完成一些联动操作。(例如每隔10秒发一次弹幕，最多发10条不同弹幕的小型独轮车，你也可以自己改动源码做成真的独轮车)(列如有5秒发言时间限制的大厅中设置5秒间隔连发文豪哥)
- 【连发模式】最佳的场景自然还是游戏中那些需要吟唱长段咒文的装逼时刻，全体频道发骚话，只要你不能尴尬，那你就是全场最靓的仔。(如下图，设置连发间隔1s，使用【300英雄】中的卫宫释放大招时的场景)

![image](https://user-images.githubusercontent.com/111073265/184251392-5f4bbf7a-471c-4085-b826-6bdfd3cda2e5.png)
![image](https://user-images.githubusercontent.com/111073265/184250915-3c870774-cb49-4828-bb5c-e55110f0e3c4.png)

# 代码开发环境配置

### 开源代码获取方式：
- [git项目页面](https://github.com/Anran-233/FasterChantDevice.git)直接打包获取
- 软件左上角logo部分鼠标右键单击即可获取开源代码

### 开发环境
- Windows系统(因为用到了大量Windows API)
- QT 5.14.2
- MSVC 2017 32bit static
- C++ 11

# 许可证声明
#### 本项目完全开源，不设任何限制
