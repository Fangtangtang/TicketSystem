# TicketSystem

## 1 功能概述

本系统为火车票订票系统。

向用户提供购票业务相关功能，包括车票查询、购票、订单操作等。
向管理员提供后台管理功能。

## 2 系统说明

### 2.1 用户相关信息

#### 2.1.1 属性

- `username`：
    - 用户的账户名，作为用户的唯一标识符。
    - 由字母开头，由字母、数字和下划线组成的字符串
    - 长度不超过 20。
- `password`：
    - 由可见字符组成的字符串
    - 长度不低于 1，不超过 30。
- `name`：
    - 用户真实姓名
    - 由 2 至 5 个汉字组成的字符串。
- `mailAddr`：
    - 同一般邮箱地址格式，仅含数字、大小写字母，@ 和 .
    - 长度不超过 30
- `privilege`：
    - 所处用户组优先级
    - 0~10 中的一个整数。

### 2.2 车次相关信息

#### 2.2.1 属性

- `trainID`：
    - 车次的唯一标识符
    - 由字母开头，字母、数字和下划线组成的字符串
    - 长度不超过 20。

- `stationNum`：
    - 车次经过的车站数量
    - 不低于 2 且不超过 100 的整数。

- `stations`：
    - 车次经过的所有车站名
    - 共 stationNum 项
    - 每个站名由汉字组成，不超过 10 个汉字。

- `seatNum`：
    - 该车次的座位数，
    - 一个不超过 100000 的整数。

- `prices`：
    - 每两站之间的票价
    - 共 (stationNum - 1) 项
    - 第 i 项表示第 i 站到第 (i+1) 站的票价，
    - 为一个不超过 100000 的整数。

- `startTime`：
    - 列车每日的发车时间。
    - 时间格式为 hh:mm ，例如 23:51

- `travelTimes`：
    - 每两站之间行车所用的时间
    - 共 (stationNum - 1) 项。
    - 单位：分钟
    - 每一项是一个不超过 10000 的整数
- `stopoverTimes`：
    - 除始发站和终点站之外，列车在每一站停留的时间
    - 共 (stationNum - 2) 项。
    - 单位：分钟
    - 每一项为一个不超过 10000 的整数
- `saleDate`：
    - 车次的售卖时间区间。
    - 由 2 项时间表示，每一项均为 2023 年 6 月至 8 月的某日期
    - 日期格式为 mm-dd，例如：06-01、08-31
- `type`：
    - 列车类型
    - 一个大写字母。

#### 2.2.2 说明

- 在时间区间（`saleDate`）内，该车次每日发一班车
- 关于 `travelTimes` 和 `stopoverTimes`：保证每一辆车从始发站到终点站用时不超过 72 小时（4320分钟）

### 2.3 指令相关信息

#### 2.3.1 指令概述

- 一条指令的格式为 `[<timestamp>] <cmd> -<key_1> <argument_1> -<key_2> <argument_2> ...`
    - 时间戳 `<timestamp>` 表示这条指令接收到的时间
        - 一个整数，递增
    - `<key>` 为一个小写字母。为简化描述，下文将在参数列表中省略 `<argument>` 并在说明中以 `-<key>` 代指其对应的参数
    - 如无特殊说明，每个 `<argument>` 都是一个不含空格的字符串
- 指令参数列表的顺序不限，可选参数可以省略。对于一些指令，可选参数具有初始值。
- 指令的返回值为命令行交互中输出在标准输出流中的执行结果字符串。
- 指令的返回值的第一行需要额外增加 `[<timestamp>]` 具体实现见实例。

#### 2.3.2 指令列表

##### `add_user`

- 参数列表
    - `-c -u -p -n -m -g`
- 返回值
    - 注册成功：`0`
    - 注册失败：`-1`
- 权限要求
    - `-c` 已登录，且新用户的权限低于 `-c` 的权限

- 说明
    - 当前用户 `-c`（`<cur_username>`），创建一个账户名为 `-u`（`<username>`），密码为 `-p`（`<password>`），用户真实姓名为 `-n`
      （`<name>`），用户邮箱为 `-m`（`<mailAddr>`），权限为 `-g`（`<privilege>`）的用户
    - 特别地，创建第一个用户时，新用户权限为 `10`，不受上述权限规则约束。具体来讲，当创建第一个用户时，忽略 `-c` 和 `-g`
      参数，并认为新用户优先级为 `10`
    - 如果 `<username>` 已经存在则注册失败

##### `login`

- 参数列表

  `-u -p`

- 说明

  用户 `-u` (`<username>`)尝试用 `-p` (`<password>`) 登录。
  若登录成功，当前用户列表增加此用户。第一次进入系统时，当前用户列表为空。
  若用户不存在或密码错误或者用户已经登录，则登陆失败。
- 返回值

  登录成功：`0`

  登录失败：`-1`

##### `logout`

- 参数列表

  `-u`

- 说明

  `-u` (`<username>`) 用户退出登录。若退出成功，将此用户移出当前用户列表。

- 返回值

  登出成功：`0`

  登出失败：`-1`

##### `query_profile`

- 参数列表

  `-c -u`

- 说明

  用户 `-c`（`<cur_username>`）查询`-u` (`<username>`) 的用户信息。

  权限要求：`-c` 已登录，且`-c` 的权限大于 `-u` 的权限，或是 `-c` 和 `-u` 相同。

- 返回值

  查询成功：一行字符串，依次为被查询用户的 `<username>`，`<name>`，`<mailAddr>` 和 `<privilege>`，用一个空格隔开。

  查询失败：`-1`

##### `modify_profile`

- 参数列表

  `-c -u (-p) (-n) (-m) (-g)`

- 说明

  用户`-c`(`<cur_username>`) 修改 `-u`(`<username>`) 的用户信息。修改参数同注册参数，且均可以省略。

  权限要求：`-c` 已登录，且`-c` 的权限大于 `-u` 的权限，或是 `-c` 和 `-u` 相同，且 `-g` 需低于 `-c` 的权限。

- 返回值

  修改成功：返回被修改用户的信息，格式同 `query_profile`。

  修改失败：`-1`

##### `add_train`

- 参数列表

  `-i -n -m -s -p -x -t -o -d -y`

    - 说明

      添加 `<trainID>` 为 `-i`，`<stationNum>` 为 `-n`，`<seatNum>` 为 `-m`，`<stations>` 为 `-s`，`<prices>` 为 `-p`
      ，`<startTime>` 为 `-x`，`<travelTimes>` 为 `-t`，`<stopoverTimes>` 为 `-o`，`<saleDate>` 为 `-d`，`<type>` 为 `-y`
      的车次。
      由于 `-s`、`-p`、`-t`、`-o` 和 `-d` 由多个值组成，输入时两个值之间以 `|` 隔开（仍是一个不含空格的字符串）。

      输入保证火车的座位数大于 0,站的数量不少于 2 不多于 100，且如果火车只有两站 `-o` 后的参数用下划线代替（见举例2）,且火车不会经过同一个站两次。
      如果 `<trainID>` 已经存在则添加失败。

    - 返回值

      添加成功：`0`

      添加失败：`-1`

    - 举例1：

      `>[666] add_train -i HAPPY_TRAINA -n 3 -m 1000 -s 上院|中院|下院 -p 114|514 -x 19:19 -t 600|600 -o 5 -d 06-01|08-17 -y G`

      `[666] 0`
    - 举例2：

      `>[667] add_train -i HAPPY_TRAINB -n 2 -m 1000 -s 上院|下院 -p 114 -x 19:19 -t 600 -o _ -d 06-01|08-17 -y G`

      `[667] 0`

##### `delete_train`

- 参数列表

  `-i`

- 说明

  删除 `<trainID>` 为 `-i` 的车次，

  删除车次必须保证未发布。

- 返回值

  删除成功：`0`

  删除失败：`-1`

##### `release_train`

- 参数列表

  `-i`

- 说明

  将车次 `-i` (`<trainID>`) 发布。
  发布前的车次，不可发售车票，无法被 `query_ticket` 和 `query_transfer` 操作所查询到；发布后的车次不可被删除。

- 返回值

  发布成功：`0`

  发布失败：`-1`

- 举例：

  `>[668] release_train -i HAPPY_TRAIN `

  `[668] 0`

##### `query_train`

- 参数列表

  `-i -d`

- 说明

  查询在日期 `-d` 发车的，车次 `-i` (`<trainID>`) 的情况，`-d` 的格式为 `mm-dd`。

- 返回值

  查询成功：输出共 `(<stationNum> + 1)` 行。

  第一行为 `<trainID> <type>`。

  接下来 `<stationNum>` 行，第 `i` 行为 `<stations[i]> <ARRIVING_TIME> -> <LEAVING_TIME> <PRICE> <SEAT>`
  ，其中 `<ARRIVING_TIME>` 和 `<LEAVING_TIME>` 为列车到达本站和离开本站的绝对时间，格式为 `mm-dd hr:mi`。`<PRICE>`
  为从始发站乘坐至该站的累计票价，`<SEAT>`
  为从该站到下一站的剩余票数。对于始发站的到达时间和终点站的出发时间，所有数字均用 `x` 代替；终点站的剩余票数用 `x`
  代替。如果车辆还未 `release` 则认为所有票都没有被卖出去。

  查询失败：`-1`

- 举例

  （上接添加列车的例子）

  `>[669] query_train -d 07-01 -i HAPPY_TRAIN`

  `[669] HAPPY_TRAIN G`

  `上院 xx-xx xx:xx -> 07-01 19:19 0 1000`

  `中院 07-02 05:19 -> 07-02 05:24 114 1000`

  `下院 07-02 15:24 -> xx-xx xx:xx 628 x`

##### `query_ticket`

- 参数列表

  `-s -t -d (-p time)`

- 说明

  查询日期为 `-d` 时从 `-s` 出发，并到达 `-t` 的车票。请注意：这里的日期是列车从 `-s` 出发的日期，不是从列车始发站出发的日期。

  `-p`的值为 `time` 和 `cost` 中的一个，若为 `time`
  表示输出按照该车次所需时间从小到大排序，否则按照票价从低到高排序。如果按照时间排序车次所需时间相同，则把 `<trainID>`
  作为第二关键字进行排序，按照票价排序；同理若出现车次票价相同，则同样把 `<trainID>` 作为第二关键字进行排序。

- 返回值

  第一行输出一个整数，表示符合要求的车次数量。

  接下来每一行输出一个符合要求的车次，按要求排序。格式为 `<trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <SEAT>`
  ，其中出发时间、到达时间格式同 `query_train`，`<FROM>` 和 `<TO>` 为出发站和到达站，`<PRICE>` 为累计价格，`<SEAT>`
  为最多能购买的票数。

- 样例

  （上接查询列车的例子）

  `>[670] query_ticket -s 中院 -t 下院 -d 08-17`

  `[670] 1`

  `HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000`

##### `query_transfer`

参数列表及其意义同 `query_ticket`。

- 说明

  在恰好换乘一次（换乘同一辆车不算恰好换乘一次）的情况下查询符合条件的车次，仅输出最优解。
  最优解的定义如下:
    * 若`(-p time)` 则总时间作为第一关键字，总价格作为第二关键字，第一辆车的 `Train ID` 作为第三关键字，第二辆车 `Train ID`
      作为第四关键字。
    * 若`(-p cost)` 则总价格作为第一关键字，总时间作为第二关键字，第一辆车的 `Train ID` 作为第三关键字，第二辆车 `Train ID`
      作为第四关键字。

  保证任意两种方案关键字均不同。
  请注意：这里的日期是列车从 `-s` 出发的日期，不是从列车始发站出发的日期。

- 返回值

  查询失败（没有符合要求的车次）：`0`

  查询成功：输出2行，换乘中搭乘的两个车次，格式同 `query_ticket`。

##### `buy_ticket`

- 参数列表
  `-u -i -d -n -f -t (-q false)`

    - 说明

      用户 `-u` (`<username>`) 购买：车次 `-i` (`<trainID>`)，日期为 `-d`，从站 `-f` 到站 `-t` 的车票 `-n` 张。

      `-q`可选 `false` 或 `true`，若为 `true`，表明在**余票不足**的情况下愿意接受候补购票，当有余票时**立即**
      视为此用户购买了车票，且保证购买的车票的数量大于 0。请注意：这里的日期是列车从 `-f` 出发的日期，不是从列车始发站出发的日期。

      权限要求：`-u` 已登录，且购买的车次必须已经被 `release`。

    - 返回值

      购买成功：一个整数，表示订单总价。

      加入候补：`queue`

      购票失败：`-1`

    - 样例

      （上接查询车票的例子，假设用户均已登录）

      `>[671] buy_ticket -u Texas -i HAPPY_TRAIN -d 08-17 -n 800 -f 中院 -t 下院`

      `[671] 411200`

      `>[672] buy_ticket -u Lappland -i HAPPY_TRAIN -d 08-16 -n 500 -f 上院 -t 下院 -q true`

      `[672] queue`

    - 样例解释

      用户 `Texas` 购买了 08-17 从中院出发的火车票 800 张，在上面的 `add_train` 操作中，08-16 从上院出发的火车会在 08-17
      到达中院，所以事实上 `Lappland` 试图购买的票的火车和 `Texas` 是同一辆车，所以 `Lappland` 购买不了 500 张票。

    - 候补队列解释：
      对于余票不足且需候补购票的订单，将加入候补队列。当有订单出现退单的时候，车次的剩余车票会增加，你需要按照候补队列下单先后顺序满足候补队列中的订单。

      ```text
      假设当前车次有100个座位，为了简单假设只有两个站，且接下来的描述均允许候补购票。
      1 buy 50
      2 buy 30
      3 buy 60
      4 buy 10
      5 buy 20
      经过五次购票订单后订单1，2，4被满足，车次还剩下10个座位，候补队列内剩下订单3和订单5。
      6 refund (2 buy 30)
      当2号订单被退票，车次内还剩下40个座位，但是候补队列中订单3需要60个座位，因此无法满足。 
      继续向后寻找能够满足的订单5，因此订单5通过候补成功购票，订单5从候补队列中被删除，并且该车次只剩下了20个座位。
      虽然订单有先后顺序，但不代表早的订单一定先完成，只是拥有更高的优先级，且订单为最小单位，不能先把部分票卖给该订单，只能完全完成一个订单。
      ```
      
##### `query_order`
- 参数列表

  `-u`

- 说明

  查询用户 `-u` (`<username>`) 的所有订单信息，按照交易时间顺序从新到旧排序（候补订单即使补票成功，交易时间也以下单时刻为准）。

  权限要求：`-u` 已登录。

- 返回值

  查询成功：第一行输出一个整数，表示订单数量。

  接下来每一行表示一个订单，格式为 `[<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>`，其中 `<NUM>` 为购票数量， `<STATUS>` 表示该订单的状态，可能的值为：`success`（购票已成功）、`pending`（位于候补购票队列中）和 `refunded`（已经退票）。

  查询失败：`-1`

- 样例

  `>[673] query_order -u Lappland`

  `[673] 1`

  `[pending] HAPPY_TRAIN 上院 08-17 05:24 -> 下院 08-17 15:24 628 500`

##### `refund_ticket`
- 参数列表

  `-u (-n 1)`

- 说明

  用户 `-u` (`<username>`）退订从新到旧（即 `query_order` 的返回顺序）第 `-n` 个 (1-base) 订单。

  权限要求：`-u` 已登录。

- 返回值

  退票成功：`0`

  退票失败：`-1`

##### `clean`
- 参数列表

  无

- 说明

  清除所有数据。

- 返回值

  `0`

##### `exit`

- 参数列表

  无

- 说明

  退出程序，所有在线用户均下线。

- 返回值

  `bye`

## 3 组成
### utility
#### 管理系统公用的类及Compare
##### 位置
tool.hpp

##### 内容
**Username**

- Username类，用户的唯一标识
- CompareUsername：字典序比较用户名

**TrainID**
- TrainID类，火车唯一标识
- CompareTrainID：字典序比较TrainID

**Train**

- Train类，含火车基础信息及火车具体信息文件地址

**Station**
- Station类，火车站基础信息

**Seat**
- Seat类，每一班车两站间的座位数

**Ticket**
- Ticket类，单张火车票信息
- Compare：
   - CompareTicket：严格比较from、to、stop_sale、start_sale
   - CompareTicketFromTo：比较from、to
   - IsAvailableTicket：比较售票日期

**Transaction**
- Transaction类，用户交易基础信息
- Compare：
  - CompareTransStrict：比较用户名和时间戳
  - CompareTransWeak：比较用户名

**TransactionDetail**
- TransactionDetail类，用户交易信息细节


#### 管理系统公用工具
##### 位置
tool.hpp

##### 内容
**Interval**
- Interval类，处理相对时间（时间间隔）

**Time**
- Time类，处理6月1日到9月的绝对时间
- CompareExactTime：比较绝对时间

**Scanner**
- Scanner类，处理"a|b|c"形式字符串，解析为string或int

#### 容器
##### vector

##### map

##### FileManager
文件管理类
在文件中读写、取地址

##### BPlusIndexTree
仅用于索引的B+树

和FileManager关联，

使用FileManager插入文件，B+树存文件中地址

##### BPlusTree
B+树

#### 其他
##### exceptions
##### pair

### head-file

## 4 Implementation
构建相应的文件和管理系统

用循环来处理命令，`processLine`

- 若读到`exit`，`flag`返回`true`退出，`return 0`
- 函数实现
  - 参数处理器处理一行指令
  - 根据指令要求进入各个系统处理



### `add_user`

`userSystem:AddUser`

- 首次添加（user的BPIT是空树），检查参数，直接插入
- 非首次，检查参数
  - 在`loginList`找`current_user`，检查优先级合法性（若未登录，优先级为负数）
  - 向BPIT中插入，若已经存在，插入失败



### `login`

`userSystem:login`

- 检查参数，检查用户合法性（已经登录？存在？密码正确？）
- 加入`loginList`



### `logout`

`loginList:logout`

- `loginList`是一个`map`，检查用户是否已经登录
- `erase`



### `query_profile`

`userASystem:QueryProfile`

- 检查参数，检查登录
- BPIT中查找，检查权限
- 输出信息



### `modify_profile`

`userSystem:ModifyProfile`

- 检查参数，检查登录
- BPIT中查找，检查权限
- `Filemanager`根据地址读取完整信息，修改，覆盖写入文件



### `add_train`

`trainSystem:AddTrain`

- 检查参数

- 截取起售、停售时间

- 向`trainTree(BPIT)`中插入基本信息

  - 成功
    - `AddStation`
      - 插入站的基础信息：站名、从出发站开始累计价格、到站相对时间、离站相对时间
    - `AddSeat`
      - 插入总天数、总站间隔数个车座信息

  

### `delete_train`

`trainSystem:DeleteTrain`

- 检查合法性
- `delete`



### `release_train`

`trainSystem:ReleaseTrain`

- 检查是否存在，released
- release
  - `trainTree.RewriteKey`
  - `ReleaseTRain`
    - 读取火车信息
    - 读取车站信息
    - 二层循环`AddTicket`(`ticket:`站名、到达时间（起售-停售）)
      - `ticketTree`(BPIT)
      - `fromTicketTree`,`toTicketTree`(BPT)：value是ticket地址



### `query_train`

`trainSystem:QueryTrain`

- 检查参数、检查是否存在
  - 存在，`PrintTrainInformation`
    - `FileMAnager`读取，将火车基础信息转化为输出信息



### `query_ticket`

- 检查参数，判断排序依据
- 从`ticketTree`找到所有符合条件的
  - `Find`找到所有符合条件的`ticket`的地址
  - `FindTicket`找到具体的票
  - 排序输出

### `query_transfer`

- 检查参数（包括时间合法性），判断排序依据
- 找可能符合要求的换乘车票

  - 从起始站到某中转站的所有合法车票存在map里

  - `FindTransferFrom`找起始站匹配的车票，使用`map[]`若中转站未出现过，新建中转站。将车票地址加入到`vec`
  - `FindTransferTo`查找到达站匹配的车票，检查起始站是否在map里。使用`map.at()`，若不存在，抛出异常；存在车票地址插入`vec`
- 构建option_vec，存所有可用选择
  - `TryTransfer`遍历map，判断是否为可用中转站
    - `CheckTransferTicket`，判断是否真正可行（时间合法）
    - 合法，插入option_vec
  - `PrintOption`，判断合法性（同一辆？），找最优解输出



### `buy_ticket`

- 检查参数，标记是否接受候补
- 检查用户是否已经登录
- 找火车，初步检查（合法时间？要求车票永远无法满足？）
- `GetStationIndex`
  - 检查车站、时间合法性
  - 计算费用
  - 引用参数`start,end`给出station的下标，`leaving_time`给出相对时间，用于计算票的起售时间
- 各种合法性的判断
- `BuyTicket`，计算到具体某天的seat，尝试买票。票不足，是否候补
- 交易成功，记录交易信息`transactionSystem.AddTransaction`
  - 若候补`waitingList.StartWaiting`
    - key：该班次第一二站seat的文件地址（用来唯一确定车次），时间戳
    - value：站的index，票数，交易信息地址



### `query_order`

- 检查参数，检查用户是否登录
- `transactionTree`找用户所有交易信息，插入vec
- 反向遍历输出要求项



### `refund_ticket`

- 检查参数，检查用户是否登录
- `transactionTree`找用户所有交易信息，插入vec
- 找到退票交易，读`transactionDetail`
- 检查交易状态
  - `refunded`
  - `success`：`RefundTicket`
    - 票回补
    - `waitingList.Rollback`
      - 所有该班次现有车票放在seat_vec
      - 找到所有该班次车的pending交易，插入vec
      - 遍历vec，`RollBack`
        - 读车票，判断是否满足，做增减，修改`transactionFile`和`waitingListTree`
      - 将回滚后seat写回文件
  - `pending`直接从`waitingList`删掉
  - 修稿交易状态，写回交易文件



### `clean`

调用`loginList`的`Clear`：将map清空

调用`userSystem`，`trainSystem`，`waitingList`，`transactionSystem`，`ticketSystem`的`Clean`：将BPT、BPIT清空

 