# PTA 试题

## 难度-中等

### 7-1 连续的子数组和

#### 7-1 题目详情

给你一个整数数组 nums 和一个整数 k ，编写一个函数来判断该数组是否含有同时满足下述条件的连续子数组：

子数组大小至少为 2 ，且子数组元素总和为 k 的倍数。

如果存在，返回 1 ；否则，返回 0 。

如果存在一个整数 n ，令整数 x 符合 `x = n * k`，则称 x 是 k 的一个倍数。0 始终视为 k 的一个倍数。

提示：

`1 <= nums.length <= 10^5`
`0 <= nums[i] <= 10^9`
`0 <= sum(nums[i]) <= 2^31 - 1`
`1 <= k <= 2^31 - 1`

输入第一行为数组 nums，第二行为整数 k

输入样例1:
在这里给出一组输入。例如：

``` text
23,2,4,6,7
6
```

输出样例1:
在这里给出相应的输出。例如：

``` text
1
```

解释：`[2,4]` 是一个大小为 2 的子数组，并且和为 6 。

输入样例2:
在这里给出一组输入。例如：

``` text
23,2,6,4,7
6
```

输出样例2:
在这里给出相应的输出。例如：

``` text
1
```

解释：`[23, 2, 6, 4, 7]` 是大小为 5 的子数组，并且和为 42 。 42 是 6 的倍数，因为 `42 = 7 * 6` 且 7 是一个整数。

输入样例3:
在这里给出一组输入。例如：

``` text
23,2,6,4,7
13
```

输出样例3:
在这里给出相应的输出。例如：

``` text
0
```

#### 7-1 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int checkSubarraySum(vector<int>& nums, int k) {
    int sum = 0;
    unordered_set<int> st;
    for (size_t i = 0; i < nums.size(); i++) {
        sum += nums[i];
        if (st.count(sum % k)) {
            return 1;
        }
        st.insert((sum - nums[i]) % k); 
    }
    return 0;
}

int main() {
    string line = "";
    getline(cin, line);
    replace(line.begin(), line.end(), ',', ' ');
    vector<int> nums = {};
    int k = 0;
    stringstream ssm(line);
    while (ssm >> k) {
        nums.push_back(k);
    }
    cin >> k;
    cout << checkSubarraySum(nums, k) << endl;
    return 0;
}
```

</details>

### 7-2 或运算的最小翻转次数

#### 7-2 题目详情

给你三个正整数 a、b 和 c。

你可以对 a 和 b 的二进制表示进行位翻转操作，返回能够使按位或运算   a OR b == c 成立的最小翻转次数。

「位翻转操作」是指将一个数的二进制表示任何单个位上的 1 变成 0 或者 0 变成 1 。

提示：

`1 <= a <= 10^9`
`1 <= b <= 10^9`
`1 <= c <= 10^9`

输入格式:

3 个正整数 a,b,c，以 "," 分隔。

输出格式:

一个数字，字符串形式

输入样例:

a,b,c分别为：

``` text
1,2,3
```

输出样例:
最小翻转次数：

``` text
0
```

#### 7-2 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int minFlipCount(int a, int b, int c) {
    int count = 0;
    for (int i=0; i < 32; ++i) {
        int x = a >> i & 1, y = b >> i & 1, z = c >> i & 1;
        if (0 == z) {
            count += x + y;
        } else {
            if (0 == x && 0 == y) {
                count += 1;
            }
        }
    }
    return count;
}

int main() {
    string line = "";
    getline(cin, line);
    replace(line.begin(), line.end(), ',', ' ');
    stringstream ssm(line);
    int a = 0, b = 0, c = 0;
    ssm >> a >> b >> c;
    cout << minFlipCount(a, b, c) << endl;
    return 0;
}
```

</details>

### 7-3 删除无效的括号

#### 7-3 题目详情

给你一个由若干括号和字母组成的字符串 s ，删除最小数量的无效括号，使得输入的字符串有效。

括号只考虑 "(" 和 ")" ，有效的括号是指一系列左括号 "(" 和 ")" 组成；但是如果有一些额外的括号，使得括号不能正确配对，就需要删除。

删除规则：从前往后，且尽可能少的删除多余括号。

输入格式:
输入一个字符串，字符串的长度小于1000。字符串中只包含字母、 "(" 和 ")"

输出格式:
输出处理后的字符串

输入样例:
在这里给出一组输入。例如：

``` text
a)())()
```

输出样例:
在这里给出相应的输出。例如：

``` text
a()()
```

#### 7-3 参考答案

我们先从左向右扫描，将多余的右括号删除，再从右向左扫描，将多余的左括号删除。

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

string minRemoveToMakeValid(const string& s) {
    string stk = "";

    int x = 0;
    for (const auto& c : s) {
        if (0 == x && ')' == c) {
            continue;
        } else if ('(' == c) {
            ++x;
        } else if (')' == c) {
            --x;
        }
        stk.push_back(c);
    }
    string ans = "";
    x = 0;
    while (stk.size()) {
        char c = stk.back();
        stk.pop_back();
        if (0 == x && '(' == c) {
            continue;
        }
        else if (')' == c) {
            ++x;
        } else if ('(' == c) {
            --x;
        }
        ans.push_back(c);
    }
    reverse(ans.begin(), ans.end());
    return ans;
}

int main() {
    string line = "";
    getline(cin, line);

    cout << minRemoveToMakeValid(line) << endl;
    
    return 0;
}
```

</details>

### 7-4 钥匙和房间

#### 7-4 题目详情

有 n 个房间，房间按从 0 到 n - 1 编号。最初，除 0 号房间外的其余所有房间都被锁住。你的目标是进入所有的房间。然而，你不能在没有获得钥匙的时候进入锁住的房间。

当你进入一个房间，你可能会在里面找到一套不同的钥匙，每把钥匙上都有对应的房间号，即表示钥匙可以打开的房间。你可以拿上所有钥匙去解锁其他房间。

给你一个数组 rooms 其中 rooms[i] 是你进入 i 号房间可以获得的钥匙集合。如果能进入所有房间返回 `true`，否则返回 `false`。

示例 1：

输入：`rooms = [[1];[2];[3];[]]` 输出：`true`

解释：我们从 0 号房间开始，拿到钥匙 1。之后我们去 1 号房间，拿到钥匙 2。然后我们去 2 号房间，拿到钥匙 3。最后我们去了 3 号房间。由于我们能够进入每个房间，我们返回 true。

示例 2：

输入：`rooms = [[1,3];[3,0,1];[2];[0]]` 输出：`false`

解释：我们不能进入 2 号房间。

提示：

`n == rooms.length`
`2 <= n <= 1000`
`0 <= rooms[i].length <= 1000`
`1 <= sum(rooms[i].length) <= 3000`
`0 <= rooms[i][j] < n`

所有 `rooms[i]`  互不相同

输入格式:
字符串表示的二维数组。例如：`[[1,3];[3,0,1];[2];[0]]`

输出格式:

小写的 `true/false`。例如：`false`。

输入样例:
在这里给出一组输入。例如：

``` text
[[1,3];[3,0,1];[2];[0]]
```

输出样例:
在这里给出相应的输出。例如：

``` text
false
```

#### 7-4 参考答案

我们可以使用深度优先搜索的方法遍历整张图，统计可以到达的节点个数，并利用数组 vis 标记当前节点是否访问过，以防止重复访问。

最后统计访问过的节点个数，若与节点总数相同则说明可以访问所有节点，否则说明存在无法到达的节点。

时间复杂度 𝑂(𝑛 + 𝑚)，空间复杂度 𝑂(𝑛)，其中 𝑛 为节点个数，而 𝑚 为边的个数。

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

bool canOpenAllRooms(const vector<vector<int>>& rooms) {
    const int n = rooms.size();
    int count = 0;
    vector<bool> v(n, false);
    function<void (int)> dfs = [&](int i){
        if (v[i]) {
            return;
        }
        v[i] = true;
        ++count;
        for (const int k : rooms[i]) {
            dfs(k);
        }
    };
    dfs(0);
    return n == count;
}

int main() {
    string line;
    getline(cin, line);
    replace(line.begin(), line.end(), '[', ' ');
    replace(line.begin(), line.end(), ']', ' ');
    replace(line.begin(), line.end(), ',', ' ');
    stringstream ssm(line);
    string room = "";
    vector<vector<int>> rooms = {};
    int key = 0;
    while (getline(ssm, room, ';')) {
        vector<int> keys = {};
        stringstream ssm1(room);
        while (ssm1 >> key) {
            keys.push_back(key);
        }
        rooms.push_back(keys);
    }

    cout << (canOpenAllRooms(rooms) ? "true" : "false") << endl;

    return 0;
}
```

</details>

### 7-5 三数之和

#### 7-5 题目详情

给你一个整数数组 nums ，判断是否存在三元组 [nums[i], nums[j], nums[k]] 满足 i != j、i != k 且 j != k ，同时还满足 nums[i] + nums[j] + nums[k] == 0 。请你返回所有和为 0 且不重复的三元组的数量。

注意：答案中不可以包含重复的三元组（例如[a,b,c]与[c,b,a]为重复），如果无符合要求的三元组，则返回0。

提示：

3 <= nums.length <= 3000

-10^5 <= nums[i] <= 10^5

运行有时间、内存限制

输入格式:
一个整数数组，每个元素其间以“空格”分隔

输出格式:
所有符合题目要求三元组的数量，如果无符合要求的三元组，则返回0。

输入样例1:
在这里给出一组输入。例如：

``` text
-1 0 1 2 -1 -4
```

输出样例1:
在这里给出相应的输出。例如：

``` text
2
```

解释：

nums[0] + nums[1] + nums[2] = (-1) + 0 + 1 = 0 。
nums[1] + nums[2] + nums[4] = 0 + 1 + (-1) = 0 。
nums[0] + nums[3] + nums[4] = (-1) + 2 + (-1) = 0 。

不重复的三元组总共有2组。

输入样例2:
在这里给出一组输入。例如：

``` text
0 1 1
```

输出样例2:
在这里给出相应的输出。例如：

``` text
0
```

解释：无符合条件的三元组

#### 7-5 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int sum3Count(vector<int>& nums) {
    sort(nums.begin(), nums.end()); // 对数组排序
    int n = nums.size();
    int ans = 0;
    for (int i=0; i < n-2 && nums[i] <= 0; ++i) { // 使用负数
        if (i && nums[i] == nums[i-1]) { // 检查去重
            continue;
        }
        int j = i+1, k = n-1; // 双指针
        while (j < k) {
            int x = nums[i] + nums[j] + nums[k];
            if (x < 0) {
                ++j;
            } else if (x > 0) {
                --k;
            } else {
                ++ans; // 此时 0 == x;
                // 指针移动
                ++j;
                --k;
                // 检查去重
                while (j<k && nums[j] == nums[j-1]) { 
                    ++j;
                }
                while (j<k && nums[k] == nums[k+1]) {
                    --k;
                }
            }
        }
    }
    return ans;
}

int main() {
    string line = "";
    getline(cin, line);
    stringstream ssm(line);
    int num = 0;
    vector<int> nums = {};
    while (ssm >> num) {
        nums.push_back(num);
    }
    cout << sum3Count(nums) << endl;
    return 0;
}
```

</details>

### 7-6 最小数

#### 7-6 题目详情

给定一组非0整数 nums，重新排列每个数的顺序（每个数不可拆分）使之组成一个最小的整数。

注意：

输入整数数组中，可能存在负数，但最多只会有一个负数

输出结果可能非常小，所以你需要返回一个字符串而不是整数。

输入格式:
一个整数数组，每个元素其间以“空格”分隔

输出格式:
最小数的字符串

输入样例1:

在这里给出一组输入。例如：

``` text
10 2
```

输出样例1:

在这里给出相应的输出。例如：

``` text
102
```

输入样例2:

在这里给出一组输入。例如：

``` text
3 30 34 5 -9
```

输出样例2:

在这里给出相应的输出。例如：

``` text
-9534330
```

#### 7-6 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

string getTargetNumer(const vector<int>& nums, function<bool (const string&, const string&)> compare) {
    if (0 == nums.size()) {
        return "";
    }
    vector<string> ss = {};
    for (int n : nums) {
        ss.push_back(to_string(n));
    }
    sort(ss.begin(), ss.end(), compare);
    if ("0" == ss[0]) {
        return "0";
    }
    string ans = "";
    for (const auto &s : ss) {
        ans += s;
    }
    return ans;
}

int main() {
    string line = "";
    getline(cin, line);
    stringstream ssm(line);
    int littleZero = 0;
    int num = 0;
    vector<int> nums = {};
    while (ssm >> num) {
        if (num < 0) {
            littleZero = num;
        } else {
            nums.push_back(num);
        }
    }

    if (littleZero < 0) {
        string ret = to_string(littleZero);
        string ans = ret + getTargetNumer(nums, [](const string& a, const string& b){
            return a + b > b + a;
        });
        cout << ans << endl;
    } else {
        string ans = getTargetNumer(nums, [](const string& a, const string& b){
            return a + b < b + a;
        });
        cout << ans << endl;
    }
    
    return 0;
}
```

</details>

### 7-7 连通网络的操作次数

#### 7-7 题目详情

用以太网线缆将 n 台计算机连接成一个网络，计算机的编号从 0 到 n-1。线缆用 connections 表示，其中 connections[i] = [a, b] 连接了计算机 a 和 b。

网络中的任何一台计算机都可以通过网络直接或者间接访问同一个网络中其他任意一台计算机。

给你这个计算机网络的初始布线 connections，你可以拔开任意两台直连计算机之间的线缆，并用它连接一对未直连的计算机。请你计算并返回使所有计算机都连通所需的最少操作次数。如果不可能，则返回 -1 。

输入格式:
第 1 行输入 n 和 m，分别表示计算机的个数和线缆个数，用空格分隔。接下来的 m 行输入，表示有线缆连接的计算机 a 和 b，用空格分隔。

输出格式:
对每一组输入，在一行中输出使所有计算机都连通所需的最少操作次数，如果不可能，则返回-1。

输入样例1:
如图所示：
例子.png

这里相应地给出一组输入：

输入样例1:

4 3
0 1
0 2
1 2

输出样例1:
在这里给出相应的输出。例如：

1

解释：拔下计算机 1 和 2 之间的线缆，并将它插到计算机 1 和 3 上。

输入样例2:

6 4
0 1
0 2
0 3
1 2

输出样例2:
在这里给出相应的输出。例如：

-1

解释：线缆数量不足。

提示

1 <= n <= 10^5

1 <= connections.length <= min(n*(n-1)/2, 10^5)

connections[i].length == 2

0 <= connections[i][0], connections[i][1] < n

connections[i][0] != connections[i][1]

没有重复的连接。

两台计算机不会通过多条线缆连接。

#### 7-7 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

struct UF {
    vector<int> p;

    UF(int n) : p(n) {
        for (int i = 0; i < n; ++i) {
            p[i] = i;
        }
    }

    int find(int x) {
        if (p[x] != x) {
            p[x] = find(p[x]);
        }
        return p[x];
    }

    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x != y) {
            p[x] = y;
        }
    }
};

int getCount(int n, const vector<vector<int>>& connections) {
    if (connections.size() < n - 1) {
        return -1; // n 台电脑至少需要 n - 1 条线
    }

    UF f(n);
    for (const auto &item : connections) {
        f.unite(item[0], item[1]);
    }

    int ans = 0;
    for (int i=0; i < n; ++i) {
        if (i == f.p[i]) {
            ans++;
        }
    }
    return ans - 1;
}

int main() {
    int n = 0, m = 0;
    vector<vector<int>> connections = {};
    cin >> n >> m;

    int l = 0, r = 0;
    while (m--) {
        vector<int> p = {};
        cin >> l >> r;
        p.push_back(l);
        p.push_back(r);
        connections.push_back(p);
    }
    
    cout << getCount(n, connections) << endl;
    
    return 0;
}
```

</details>

### 7-8 分段反转链表

#### 7-8 题目详情

给定一个常数 K 和一个单链表 L，请你在单链表上每 K 个元素做一次反转，并输出反转完成后的链表。

如果链表最后一部分不足 K 个元素，则最后一部分不翻转。

例如，假设 L 为 1→2→3→4→5→6

如果 K=3，则你应该输出 3→2→1→6→5→4

如果 K=4，则你应该输出 4→3→2→1→5→6

输入格式:
第一行包含头节点地址，总节点数量 N 以及常数 K。1≤N≤100000，1≤K≤N 。

节点地址用一个 5 位非负整数表示（可能有前导 0），NULL 用 −1 表示。

接下来 N 行，每行描述一个节点的信息，格式如下：

Address Data Next

其中 Address 是节点地址，Data 是一个绝对值不超过100000的整数，Next 是下一个节点的地址。

输出格式:
将重新排好序的链表，从头节点开始，依次输出每个节点的信息，格式与输入相同。

输入样例:
在这里给出一组输入。例如：

``` text
00100 6 4
00000 4 99999
00100 1 12309
68237 6 -1
33218 3 00000
99999 5 68237
12309 2 33218
```

输出样例:
在这里给出相应的输出。例如：

``` text
00000 4 33218
33218 3 12309
12309 2 00100
00100 1 99999
99999 5 68237
68237 6 -1
```

#### 7-8 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    vector<long> L, S;
    long start, N, K, add, data[100001], next[100001];
    cin >> start >> N >> K;
    while (N--) {
        cin >> add;
        cin >> data[add] >> next[add];
    }
    
    while (-1 != start) {
        L.push_back(start);
        start = next[start];
    }

    if (L.size() > 0) {
        size_t i = 0;
        while (i < L.size() && (L.size() - i) >= K) {
            reverse(L.begin() + i, L.begin() + i + K);
            i += K;
        }
        printf("%05ld %ld ", L.front(), data[L.front()]);
        L.erase(L.begin());
        while (L.size() > 0) {
            printf("%05ld\n%05ld %ld ", L.front(), L.front(), data[L.front()]);
            L.erase(L.begin());
        }
        printf("-1");
    }
    return 0;
}
```

</details>

### 7-9 跳跃距离

#### 7-9 题目详情

给定一个非负整数数组nums，最初位于数组的第一个位置；数组中的每个元素表示你在该位置可以跳跃的最大长度。

请确定是否可以到达最后一个位置。

输入格式:
每个测试用例一行，以“,”分隔，代表nums数组

输出格式:
请确定是否可以到达最后一个位置。如果能，返回true，否则返回false

输出样例1：
在这里给出一组输入。例如：

``` text
2,3,1,1,4
```

输出样例1：
在这里给出相应的输出。例如：

``` text
true
```

输入样例2:
在这里给出一组输入。例如：

``` text
3,2,1,0,4
```

输出样例2:
在这里给出相应的输出。例如：

``` text
false
```

#### 7-9 参考答案

想象你是那个在格子上行走的小人，格子里面的数字代表“能量”，你需要“能量”才能继续行走。

每次走到一个格子的时候（消耗一个能量），你检查现在格子里面的“能量”和你自己拥有的“能量”哪个更大，取更大的“能量”！ 如果你有更多的能量，你就可以走的更远啦！~

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

string canJumpToEnd(const vector<int>& nums) {
    if (0 == nums.size()) {
        return "true";
    }
    size_t power = nums[0];
    size_t step = 1;
    while (power != 0 && step < nums.size()) {
        --power;
        if (power < nums[step]) {
            power = nums[step];
        }
        ++step;
    }
    return step == nums.size() ? "true" : "false";
}

int main() {
    string line = "";
    getline(cin, line);
    vector<int> nums = {};
    string num;
    stringstream ssm(line);
    while (getline(ssm, num, ',')) {
        nums.push_back(stoi(num));
    }

    cout << canJumpToEnd(nums) << endl;

    return 0;
}
```

</details>

### 7-10 验证栈序列

#### 7-10 题目详情

给定 pushed 和 popped 两个序列，每个序列中的 值都不重复，只有当它们可能是在最初空栈上进行的推入 push 和弹出 pop 操作序列的结果时，返回 true；否则，返回 false 。

输入格式:
第一行为输入序列，第二行为输出序列

输出格式:
true 或者 false

输入样例1:
在这里给出一组输入。例如：

``` text
1,2,3,4,5
4,5,3,2,1
```

输出样例1:
在这里给出相应的输出。例如：

``` text
true
```

解释1:
我们可以按以下顺序执行：

push(1), push(2), push(3), push(4), pop() -> 4,
push(5), pop() -> 5, pop() -> 3, pop() -> 2, pop() -> 1

输入样例2:
在这里给出一组输入。例如：

``` text
1,2,3,4,5
4,3,5,1,2
```

输出样例2:
在这里给出相应的输出。例如：

``` text
false
```

解释2:
1 不能在 2 之前弹出。

提示:
1 <= pushed.length <= 1000
0 <= pushed[i] <= 1000

pushed 的所有元素 互不相同

popped.length == pushed.length

popped 是 pushed 的一个排列

返回的字符串需要是小写的！

#### 7-10 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

string checkResult(const vector<int>& pushed, const vector<int>& popped) {
    stack<int> stk;
    size_t i = 0;
    for (const auto x : pushed) {
        stk.push(x);
        while (stk.size() > 0 && stk.top() == popped[i]) {
            stk.pop();
            ++i;
        }
    }
    return i == popped.size() ? "true" : "false";
}

int main() {
    string line = "";
    getline(cin, line);
    vector<int> pushed = {};
    stringstream ssm0(line);
    string num = "";
    while(getline(ssm0, num, ',')) {
        pushed.push_back(stoi(num));
    }

    line = "";
    getline(cin, line);
    vector<int> popped = {};
    stringstream ssm1(line);
    num = "";
    while(getline(ssm1, num, ',')) {
        popped.push_back(stoi(num));
    }

    cout << checkResult(pushed, popped) << endl;
    
    return 0;
}
```

</details>

### 7-11 二叉树的中序遍历

#### 7-11 题目详情

给定一个二叉树的根节点root，返回它的中序遍历结果。

输入格式:
给定一个二叉树的根节点root。
树上的节点数满足 0 <= n <= 1000, 每个节点的值满足 -1000 <= val <= 1000

输出格式:
输出中序遍历后结果。

输入样例:
在这里给出一组输入。例如：

``` text
1,null,2,3
```

以上输入用例构建的二叉树如下图所示：

输出样例:
在这里给出相应的输出。例如：

中序遍历后，结果输出为：1，3，2

``` text
1,3,2
```

#### 7-11 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

vector<int> ans;

struct TreeNode {
    int val = 0;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    TreeNode(const string& s) : val(stoi(s)) {}
};

void dfs(TreeNode *root) {
    if (!root) {
        return;
    }
    if (root->left) {
        dfs(root->left);
    }
    ans.push_back(root->val);
    if (root->right) {
        dfs(root->right);
    }
}

TreeNode* createBinTree(vector<string>& nums) {
    if (0 == nums.size() || "null" == nums[0]) {
        return nullptr;
    }
    size_t i = 0;
    TreeNode* root = new TreeNode(nums[i]);
    ++i;
    vector<TreeNode*> lastNodes = {};
    lastNodes.push_back(root);
    while (i < nums.size()) {
        vector<TreeNode*> currentNodes = {};
        for (auto lastNode : lastNodes) {
            // left
            if (i < nums.size() && "null" != nums[i]) {
                TreeNode* node = new TreeNode(nums[i]);
                lastNode->left = node;
                currentNodes.push_back(node);
            }
            ++i;
            // right
            if (i < nums.size() && "null" != nums[i]) {
                TreeNode* node = new TreeNode(nums[i]);
                lastNode->right = node;
                currentNodes.push_back(node);
            }
            ++i;
        }
        lastNodes = currentNodes;
    }
    return root;
}

int main() {
    string line = "";
    getline(cin, line);
    stringstream ssm(line);
    vector<string> nums;
    string s = "";
    while(getline(ssm, s, ',')) {
        nums.push_back(s);
    }

    TreeNode *root = createBinTree(nums);

    if (root) {
        dfs(root);
    }

    while (ans.size() > 1) {
        cout << ans.front() << ",";
        ans.erase(ans.begin());
    }
    if (ans.size() > 0) {
        cout << ans.front();
    }
    cout << endl;
}
```

</details>

### 7-12 存在重复元素3

#### 7-12 题目详情

给你一个整数数组 nums 和两个整数 indexDiff 和 valueDiff 。

找出满足下述条件的下标对 (i, j)：

i != j

abs(i - j) <= indexDiff
abs(nums[i] - nums[j]) <= valueDiff

如果存在，返回 1 ；否则，返回 0

提示:

2 <= nums.length <= 10^5
-10^9 <= nums[i] <= 10^9
1 <= indexDiff <= nums.length
0 <= valueDiff <= 10^9

运行有时间和内存限制

输入格式:
字符串，包括3部分：数组nums、indexDiff、valueDiff，每部分用英文逗号分隔。

其中，数组nums的每个元素用空格分隔。

输出格式:
0 or 1

输入样例1:
在这里给出一组输入。例如：

``` text
1 2 3 1,3,0
```

输出样例1:
在这里给出相应的输出。例如：

``` text
1
```

解释：可以找出 (i, j) = (0, 3) 。满足下述 3 个条件：
i != j --> 0 != 3
abs(i - j) <= indexDiff --> abs(0 - 3) <= 3
abs(nums[i] - nums[j]) <= valueDiff --> abs(1 - 1) <= 0

输入样例2:
在这里给出一组输入。例如：

``` text
1 5 9 1 5 9,2,3
```

输出样例2:
在这里给出相应的输出。例如：

``` text
0
```

解释：尝试所有可能的下标对 (i, j) ，均无法满足这 3 个条件，因此返回 0

#### 7-12 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

// 将题目条件翻译一下就行
int getResult(const vector<long>& nums, long indexDiff, long valueDiff) {
    for (size_t i=0; i < nums.size(); i++){
        for (size_t j = i+1; j < i + indexDiff + 1 && j < nums.size(); j++){
            long diff = abs(nums[i] - nums[j]);
            if (diff <= valueDiff){
                return 1;
            }
        }
    }
    return 0;
}

int main() {
    string line;
    getline(cin, line);
    stringstream ssm(line);
    vector<string> inputs;
    string numStr = "";
    while (getline(ssm, numStr, ',')) {
        inputs.push_back(numStr);
    }

    vector<long> nums;
    stringstream ssl(inputs[0]);
    long num = 0;
    while (ssl >> num) {
        nums.push_back(num);
    }
    long indexDiff = stol(inputs[1]);
    long valueDiff = stol(inputs[2]);

    cout << getResult(nums, indexDiff, valueDiff) << endl;
    return 0;
}
```

</details>

### 7-13 删除有序链表中重复2次以上元素

#### 7-13 题目详情

给出一个升序排序的链表 L，删除链表中重复2次以上出现的元素，只保留原链表中出现一次或重复2次 的元素。

例如：

给出的链表为 1→2→3→3→4→4→4→4→5, 返回 1→2→3→3→5.

给出的链表为1→1→1→2→3, 返回 2→3.

数据范围： 链表长度 0≤size≤100000，链表中的值满足 ∣val∣≤10000。

输入格式:
第一行包含头节点地址，总节点数量 N （1≤N≤100000）

节点地址用一个 5 位非负整数表示（可能有前导 0），NULL 用 −1 表示。

接下来 N 行，每行描述一个节点的信息，格式如下：

Address Data Next

其中 Address 是节点地址，Data 是一个绝对值不超过100000的整数，Next 是下一个节点的地址。

输出格式:
输出删除有序链表中重复2次以上元素后的链表。每个结点占一行，按输入的格式输出。

输入样例:
在这里给出一组输入。例如：

``` text
00100 10
99999 3 87654
87654 4 11111
55555 8 -1
44444 4 55555
23854 2 00000
11111 4 22222
00100 1 23854
22222 4 33333
00000 3 99999
33333 4 44444
```

输出样例:
在这里给出相应的输出。例如：

``` text
00100 1 23854
23854 2 00000
00000 3 99999
99999 3 55555
55555 8 -1
```

#### 7-13 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    vector<long> L, S;
    long start = 0, N = 0, add = 0, data[100001], next[100001];

    // 获取输入
    cin >> start >> N;
    while (N--) {
        cin >> add;
        cin >> data[add] >> next[add];
    }

    // 获取完整的链表
    while (-1 != start) {
        L.push_back(start);
        start = next[start];
    }

    // 统计数据出现次数
    int count[sizeof data + 1];
    memset(count, 0, sizeof data);
    for (const auto& node : L) {
        count[data[node]] += 1;
    }

    // 获取剔除不符合要求的数据的链表
    for (const auto& node : L) {
        if (count[data[node]] <= 2) {
            S.push_back(node);
        }
    }

    // 打印结果
    if (!S.empty()) {
        printf("%05ld %ld ", S.front(), data[S.front()]);
        S.erase(S.begin());
        while (!S.empty()) {
            printf("%05ld\n%05ld %ld ", S.front(), S.front(), data[S.front()]);
            S.erase(S.begin());
        }
        printf("-1");
    }
    return 0;
}
```

</details>

### 7-14 按格式重排链表

#### 7-14 题目详情

给定一个单链表L1​→L2​→⋯→Ln−1​→Ln​

请编写程序将链表重新排列为Ln​→L1​→Ln−1​→L2​→…

例如：给定L为1→2→3→4→5→6，则输出应该为6→1→5→2→4→3

输入格式：
第一行包含头节点地址，总节点数量 N （1≤N≤100000）

节点地址用一个 5 位非负整数表示（可能有前导 0），空地址 NULL 用 −1 表示。

接下来 N 行，每行描述一个节点的信息，格式如下：

Address Data Next

其中 Address 是节点地址，Data 是一个绝对值不超过100000的整数，Next 是下一个节点的地址。

题目保证给出的链表上至少有两个结点。

输出格式：
对每个测试用例，顺序输出重排后的结果链表，其上每个结点占一行，格式与输入相同。

输入样例:
在这里给出一组输入。例如：

``` text
00100 6
00000 4 99999
00100 1 12309
68237 6 -1
33218 3 00000
99999 5 68237
12309 2 33218
```

输出样例:
在这里给出相应的输出。例如：

``` text
68237 6 00100
00100 1 99999
99999 5 12309
12309 2 00000
00000 4 33218
33218 3 -1
```

#### 7-14 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    vector<long> L;
    long start, N, add, data[100001], next[100001];
    cin >> start >> N;

    while (N--) {
        cin >> add;
        cin >> data[add] >> next[add];
    }

    while (-1 != start) {
        L.push_back(start);
        start = next[start];
    }

    printf("%05ld %ld ", L.back(), data[L.back()]);
    L.pop_back();
    size_t n = 0;
    while (L.size() > 0) {
        if (0 == (n++)%2) {
            printf("%05ld\n%05ld %ld ", L.front(), L.front(), data[L.front()]);
            L.erase(L.begin());
        } else {
            printf("%05ld\n%05ld %ld ", L.back(), L.back(), data[L.back()]);
            L.pop_back();
        }
    }
    printf("-1");
    return 0;
}
```

</details>

### 7-15 字符串最长公共子序列

#### 7-15 题目详情

假设你正在使用一款版本控制系统，这款系统使用两个字符串A和B来表示两个版本内容。这两个字符串的长度都不超过1000。你的任务是计算出这两个版本之间的最长公共子序列长度，以便更好地理解这两个版本之间的差异。请注意，子序列的字符不需要在原始字符串中连续。

例如，假设输入两个版本，其内容分别为 "abcfbcab" 和 "bdcabdfcab"，那么 "bca" 是一个公共子序列，"abcab" 也是一个公共子序列，其中 "abfcab" 是这两个版本之间的最长公共子序列，输出的长度为6

输入格式:
第1行：表示第一个版本代码文件的字符串

第2行：表示第二个版本代码文件的字符串

输出格式:
输出这两个版本的最长公共子序列长度

输入样例1:
在这里给出一组输入。例如：

``` text
abcfbcab
bdcabdfcab
```

输出样例1:
在这里给出相应的输出。例如：

``` text
6
```

输入样例2:
在这里给出一组输入。例如：

``` text
ABCBDAB
BDCAB
```

输出样例2:
在这里给出相应的输出。例如：

``` text
4
```

#### 7-15 参考答案

解乏：动态规划

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

size_t getLengthOfCommonSubString(const string& str1, const string& str2) {
    size_t m = str1.size();
    size_t n = str2.size();
    size_t f[m + 1][n + 1];
    memset(f, 0, sizeof f);
    for (size_t i=1; i <= m; ++i) {
        for (size_t j=1; j <= n; ++j) {
            if (str1[i-1] == str2[j-1]) {
                f[i][j] = f[i-1][j-1] + 1;
            } else {
                f[i][j] = max(f[i][j-1], f[i-1][j]);
            }
        }
    }
    return f[m][n];
}

int main() {
    string str1 = "";
    getline(cin, str1);
    string str2 = "";
    getline(cin, str2);
    cout << getLengthOfCommonSubString(str1, str2) << endl;
    return 0;
}
```

</details>

### 7-16 Call to your teacher

#### 7-16 题目详情

从实验室出来后，你忽然发现你居然把自己的电脑落在了实验室里，但是实验室的老师已经把大门锁上了。

更糟的是，你没有那个老师的电话号码。你开始给你知道的所有人打电话，询问他们有没有老师的电话，如果没有，他们也会问自己的同学来询问电话号码。

那么，你能联系到老师并且拿到电脑吗？

输入格式:
存在多组测试样例

每组样例的第一行分别是两个整数n(1<n<=50)，m(1<m<=2000)，n是在题目当中出现的人数，其中你的序号是1号，实验室老师的序号是n。
接下来的m行，每行有两个整数x(1<=x<=n)，y(1<=y<=n)，代表x有y的电话号码。

输出格式:
对于每组测试样例，如果你最终能联系到老师，输出“Yes”，否则输出“No”。

输入样例1:
在这里给出一组输入。例如：

``` text
5 5
1 3
2 3
3 4
2 4
4 5
```

输出样例1:
在这里给出相应的输出。例如：

``` text
Yes
```

输入样例2:
在这里给出一组输入。例如：

``` text
4 3
1 2
2 3
4 1
```

输出样例2:
在这里给出相应的输出。例如：

``` text
No
```

#### 7-16 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

bool canCallToTeacher(const vector<pair<int, int> > & datas, int n) {

    // 构建有向图
    vector<vector<int>> graph(n + 1);
    for (const auto& data : datas) {
        graph[data.first].push_back(data.second);
    }

    vector<bool> visited(n + 1, false);

    // BFS 遍历有向图
    queue<int> q;
    q.push(1);
    visited[1] = true;

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        if (current == n) {
            return true;
        }

        for (int next : graph[current]) {
            if (!visited[next]) {
                visited[next] = true;
                q.push(next);
            }
        }
    }

    return false;
}

int main() {
    int n = 0, m = 0;
    cin >> n >> m;
    vector<pair<int, int>> datas(m);
    for (int i = 0; i < m; ++i) {
        cin >> datas[i].first >> datas[i].second;
    }
    if (canCallToTeacher(datas, n)) {
        cout << "Yes" << endl;
    } else {
        cout << "No" << endl;
    }
    return 0;
}
```

</details>

### 7-17 根据数字的补数排序

#### 7-17 题目详情

对整数的二进制表示取反（0 变 1 ，1 变 0）后，再转换为十进制表示，可以得到这个整数的补数。

例如，整数 5 的二进制表示是 "101" （没有前导零位），取反后得到 "010" ，再转回十进制表示得到补数 2 。

给你一个整数数组 arr 。请你将数组中的元素按照其补数升序排序。如果补数相同，则按照原数值大小升序排列。

请你返回排序后的数组。

提示：

1 <= arr.length <= 500

0 <= arr[i] <= 10^4

输入格式:
整数数组arr，以",”分隔字符串的形式作为输入

输出格式:
排好序的整数数组，以",”分隔字符串的形式作为输出

输入样例:
原始数组arr：

``` text
5,10,4,2
```

输出样例:
排序后的arr：

``` text
2,5,4,10
```

#### 7-17 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int getBu(int num) {
    if (0 == num) {
        return 1;
    }
    int tmp = num, c = 0;
    while (tmp > 0) {
        tmp >>= 1;
        c = (c << 1) + 1;
    }
    return num ^ c; // 统计num有多少位，与c异或即可
}

bool compare(int a, int b) {
    int ba = getBu(a);
    int bb = getBu(b);
    if (ba == bb) {
        return a < b;
    }
    return ba < bb;
}

int main() {
    string line = "";
    getline(cin, line);
    stringstream ssm(line);
    string num = "";
    vector<int> nums;
    while (getline(ssm, num, ',')) {
        nums.push_back(stoi(num));
    }

    sort(nums.begin(), nums.end(), compare);

    while (nums.size() > 1) {
        cout << nums.front() << ",";
        nums.erase(nums.begin());
    }
    if (nums.size() > 0) {
        cout << nums.front();
    }
    cout << endl;
    
    return 0;
}
```

</details>

### 7-18 连续数列

#### 7-18 题目详情

给定一个整数数组，找出总和最大的连续数列，并返回总和。

进阶：如果你已经实现复杂度为 O(n) 的解法，尝试使用更为精妙的分治法求解。

输入格式:
数组nums

输出格式:
连续子数组的最大和

输入样例:
在这里给出一组输入。例如：

``` text
-2,1,-3,4,-1,2,1,-5,4
```

输出样例:
在这里给出相应的输出。例如：

``` text
6
```

#### 7-18 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    string line;
    getline(cin, line);
    stringstream ssm(line);
    string num = "";
    int ret = INT_MIN;
    int sum = 0;
    while (getline(ssm, num, ',')) {
        sum = max(0, sum);
        sum += stoi(num);
        ret = max(ret, sum);
    }
    cout << ret << endl;
    return 0;
}
```

</details>
