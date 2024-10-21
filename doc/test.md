## 难度-困难

### 7-1 按格式合并两个链表

#### 7-1 题目详情

给定两个链表L1​=a1​→a2​→⋯→an−1​→an​ 和L2​=b1​→b2​→⋯→bm−1​→bm​，其中n≥2m。

需要将较短的链表L2​反转并合并到较长的链表L1​中

使得合并后的链表如下形式：a1​→a2​→bm​→a3​→a4​→bm−1​→…

合并规则：在长链表中每隔两个元素，将短链表中的元素倒序插入。

例如：给定一个较长链表1→2→3→4→5，另外一个较短链表6→7，需要输出1→2→7→3→4→6→5

输入格式:
第一行包含两个链表的第一个节点地址（不确定哪个链表更长），以及两个链表的总节点数N(≤100000)。

节点地址用一个 5 位非负整数表示（可能有前导 0），空地址 NULL 用 −1 表示。

例如：00100 01000 7。其中00100表示第一个链表的首个节点地址，01000表示第二个链表的首个节点地址，7表示两个链表的总节点数。

接下来N行，每行描述一个节点的信息，格式如下：

Address Data Next

其中 Address 是节点地址，Data 是一个绝对值不超过100000的整数，Next 是下一个节点的地址。

保证两个链表都不为空，且较长的链表至少是较短链表长度的两倍。

输出格式:
对于每个测试用例，按顺序输出合并后的结果链表。每个结点占一行，按输入的格式输出。

输入样例:
在这里给出一组输入。例如：

00100 01000 7
02233 2 34891
00100 6 00001
34891 3 10086
01000 1 02233
00033 5 -1
10086 4 00033
00001 7 -1
输出样例:
在这里给出相应的输出。例如：

01000 1 02233
02233 2 00001
00001 7 34891
34891 3 10086
10086 4 00100
00100 6 00033
00033 5 -1

#### 7-1 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <vector>
#include <iostream>
using namespace std;

void output(vector<long>& longLink, vector<long>& shortLink, long data[]) {
    printf("%05ld %ld ", longLink.front(), data[longLink.front()]);
    longLink.erase(longLink.begin());
    for (long z = 2; !longLink.empty() || !shortLink.empty(); ++z) {
        if (0 == z%3 && !shortLink.empty()) {
            printf("%05ld\n%05ld %ld ", shortLink.back(), shortLink.back(), data[shortLink.back()]);
            shortLink.pop_back();
        } else {
            printf("%05ld\n%05ld %ld ", longLink.front(), longLink.front(), data[longLink.front()]);
            longLink.erase(longLink.begin());
        }
    }
    printf("-1");
}

int main() {
    vector<long> link1, link2;
    long start1, start2, total, data[100001], next[100001], currentAddress;
    cin >> start1 >> start2 >> total;

    while (total--) {
        cin >> currentAddress;
        cin >> data[currentAddress] >> next[currentAddress];
    }

    while (-1 != start1) {
        link1.push_back(start1);
        start1 = next[start1];
    }

    while (-1 != start2) {
        link2.push_back(start2);
        start2 = next[start2];
    }

    if (link1.size() > link2.size()) {
        output(link1, link2, data);
    } else {
        output(link2, link1, data);
    }
    
    return 0;
}
```

</details>

### 7-2 按公因数计算最大组件大小

#### 7-2 题目详情

给定一个由不同正整数组成的非空数组 nums，考虑下面的构图：

有 nums.length 个节点，按照从 nums[0]到 nums[nums.length-1]标记；

只有当 nums[i] 和 nums[j] 共用一个大于 1 的公因数时，nums[i] 和 nums[j] 之间才有一条边。

返回构图中最大连通组件的大小。

输入格式:
输入为数组元素，空格分隔

输出格式:
输出最大连通组件的大小

输入样例1:
在这里给出一组输入。例如：

``` text
4 6 15 35
```

对应的构图为

``` mermaid
graph TB
A((4))
B((6))
C((15))
D((35))

A-->B
B-->C
C-->D
```

可以看到，最大连通组件的大小为 4。

输出样例1:
在这里给出相应的输出。例如：

``` text
4
```

输入样例2:
在这里给出一组输入。例如：

20 50 9 63

对应的构图为

可以看到，最大连通组件的大小为 2。

输出样例2:
在这里给出相应的输出。例如：

2

输入样例3:
在这里给出一组输入。例如：

2 3 6 7 4 12 21 39

对应的构图为

可以看到，最大连通组件的大小为 8。

输出样例3:
在这里给出相应的输出。例如：

8

注意：
1 <= nums.length <= 2*10^4

1 <= nums[i] <= 10^5

nums 中所有值都不同

#### 7-2 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

class UnionFind {
public:
    vector<int> p;

    UnionFind(int _n)
        : p(_n) {
        for (int i = 0; i < _n; ++i) {
            p[i] = i;
        }
    }

    void unite(int a, int b) {
        int pa = find(a);
        int pb = find(b);
        if (pa != pb) {
            p[pa] = pb;
        }
    }

    int find(int x) {
        if (p[x] != x) {
            p[x] = find(p[x]);
        }
        return p[x];
    }
};

int main() {
    string input = "";
    getline(cin, input);
    vector<int> nums;
    int num = 0;
    stringstream ssm(input);
    while (ssm >> num) {
        nums.push_back(num);
    }

    int n = *max_element(nums.begin(), nums.end());
    UnionFind* uf = new UnionFind(n + 1);
    for (int v : nums) {
        int i = 2;
        while (i <= v / i) {
            if (v % i == 0) {
                uf->unite(v, i);
                uf->unite(v, v / i);
            }
            ++i;
        }
    }
    vector<int> count(n + 1);
    int ans = 0;
    int t = 0;
    for (int v : nums) {
        t = uf->find(v);
        ++count[t];
        ans = max(ans, count[t]);
    }

    cout << ans << endl;
    
    return 0;
}
```

</details>

### 7-3 拼接最大数

#### 7-3 题目详情

给定长度分别为 m 和 n 的两个数组，其元素由 0-9 构成，表示两个自然数各位上的数字。

现在从这两个数组中选出 k (0 <=k <= m + n) 个数字拼接成一个新的数，要求从同一个数组中取出的数字保持其在原数组中的相对顺序。
求满足该条件的最大数。结果返回一个表示该最大数的长度为 k 的数组。

输入格式:
输入三个行内容：

第一行是数组nums1，元素内容用逗号分隔；数组最大长度为1000。

第二行是数组nums2，元素内容用逗号分隔；数组最大长度为1000。

第三行是长度k；

输出格式:
返回一个表示该最大数的长度为 k 的数组，数组元素用逗号隔开。

输入样例:
在这里给出一组输入。例如：

``` text
3,4,6,5
9,1,2,5,8,3
5
```

输出样例:
在这里给出相应的输出。例如：

``` text
9,8,6,5,3
```

提示：
1 <= nums1.length, nums2.length <= 1000

0 <= nums1[i], nums2[i] <= 9

#### 7-3 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

vector<int> mergeMax(const vector<int>& nums1,const vector<int>& nums2) {
    vector<int> ret = {};
    size_t i = 0;
    size_t j = 0;
    size_t n = nums1.size() + nums2.size();

    for (size_t k = 0; k < n; k++) {
        if (i < nums1.size() && j < nums2.size()) {
            if (nums1[i] >= nums2[j]) {
                ret.push_back(nums1[i++]);
            } else {
                ret.push_back(nums2[j++]);
            }
        } else if (i < nums1.size()) {
            ret.push_back(nums1[i++]);
        } else if (j < nums2.size()) {
            ret.push_back(nums2[j++]);
        }
    }
    return ret;
}

vector<int> maxSubNums(const vector<int>& nums, size_t n) {
    vector<int> ret = {};
    int jump = nums.size() - n;
    for(auto num : nums) {
        while(jump > 0 && ret.size() > 0 && ret.back() < num) {
            ret.pop_back();
            jump--;
        }
        ret.push_back(num);
    }
    ret.resize(n);
    return ret;
}

bool bigger(const vector<int>& nums1, const vector<int>& nums2) {
    if (nums1.size() != nums2.size()) {
        return nums1.size() > nums2.size();
    }
    for (size_t i = 0; i < nums1.size(); ++i) {
        if (nums1[i] != nums2[i]) {
            return nums1[i] > nums2[i];
        }
    }
    return false;
}

int main() {
    string line = "";
    getline(cin, line);
    stringstream ssm(line);
    vector<int> nums1 = {};
    string num = "";
    while(getline(ssm, num, ',')) {
        nums1.push_back(stoi(num));
    }

    line = "";
    getline(cin, line);
    stringstream ssm1(line);
    vector<int> nums2 = {};
    num = "";
    while(getline(ssm1, num, ',')) {
        nums2.push_back(stoi(num));
    }

    line = "";
    size_t k = 0;
    getline(cin, line);
    k = stoi(line);

    vector<int> ret = {};
    for (size_t i = 0; i < k; ++i) {
        if (i > nums1.size() || (k - i) > nums2.size()) {
            continue;
        }
        vector<int> r = mergeMax(maxSubNums(nums1, i), maxSubNums(nums2, k - i));
        if (bigger(r, ret)) {
            ret = r;
        }
    }

    while (ret.size() > 1) {
        cout << ret.front() << ",";
        ret.erase(ret.begin());
    }
    if (ret.size() > 0) {
        cout << ret.front();
    }
    cout << endl;
    return 0;
}
```

</details>

### 7-4 最长递增子序列

#### 7-4 题目详情

给你一个整数数组nums，找到其中最长严格递增子序列的长度。

子序列 是由数组派生而来的序列，删除（或不删除）数组中的元素而不改变其余元素的顺序。例如，[3,6,2,7] 是数组 [0,3,1,6,2,2,7] 的子序列。

输入格式:
1 <= nums.length <= 2000
-10000 <= nums[i] <= 10000

输出格式:
最长严格递增子序列的长度

输入样例:
在这里给出一组输入。例如：

``` text
10 9 2 5 3 7 101 18
```

输出样例:
在这里给出相应的输出。例如：

``` text
4
```

#### 7-4 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int maxSubListLength(const vector<int>& nums) {
    size_t n = nums.size();
    if (1 == n) {
        return 1;
    }
    vector<int> f(n, 1);
    for (size_t i=1; i < n; ++i) {
        for (size_t j=0; j < i; ++j) {
            if (nums[j] < nums[i]) {
                f[i] = max(f[i], f[j] + 1);
            }
        }
    }
    return *max_element(f.begin(), f.end());
}

int main() {
    string input = "";
    getline(cin, input);
    stringstream ssm(input);
    vector<int> nums;
    int num = 0;
    while (ssm >> num) {
        nums.push_back(num);
    }
    cout << maxSubListLength(nums) << endl;
    return 0;
}
```

</details>

### 7-5 二叉树的最大路径和

#### 7-5 题目详情

二叉树中的 路径 被定义为一条节点序列，序列中每对相邻节点之间都存在一条边。同一个节点在一条路径序列中 至多出现一次 。该路径 至少包含一个 节点，且不一定经过根节点。
路径和 是路径中各节点值的总和。
给你一个二叉树的根节点 root ，返回其 最大路径和 。

输入格式:
树上的节点数满足 0 <= n <= 1000, 每个节点的值满足 -1000 <= val <= 1000

（注：null节点的左右叶子不会再打印null）

输出格式:
输出最大路径的和

输入样例:
在这里给出一组输入。例如：

``` text
-10,9,20,null,null,15,7
```

以上样例构造的二叉树如下图所示：

``` mermaid
graph TB
A(((-10)))
B((9))
C((20))
D((15))
E((7))
A-->B
A-->C
C-->D
C-->E
```

输出样例:
在这里给出相应的输出。例如：

``` text
42
```

#### 7-5 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int maxSum = INT_MIN;

struct TreeNode {
    int val = 0;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    TreeNode(string s):val(stoi(s)){}

    int getMaxNum() {
        int maxLeft = 0;
        int maxRight = 0;
        if (nullptr != left) {
            maxLeft = max(0, left->getMaxNum());
        }
        if (nullptr != right) {
            maxRight = max(0, right->getMaxNum());
        }
        maxSum = max(maxSum, val + maxLeft + maxRight);
        return val + max(maxLeft, maxRight);
    }
};

TreeNode* createBinTree(const vector<string>& values) {
    if (0 == values.size() || "null" == values[0]) {
        return nullptr;
    }
    size_t i = 0;
    TreeNode* ret = new TreeNode(values[i]);
    ++i;
    vector<TreeNode*> lastNodes;
    lastNodes.push_back(ret);

    while (i < values.size()) {
        vector<TreeNode*> currentNodes;
        for (auto lastNode : lastNodes) {
            // left
            if (i < values.size() && "null" != values[i]) {
                TreeNode* node = new TreeNode(values[i]);
                lastNode->left = node;
                currentNodes.push_back(node);
            }
            ++i;
            // right
            if (i < values.size() && "null" != values[i]) {
                TreeNode* node = new TreeNode(values[i]);
                lastNode->right = node;
                currentNodes.push_back(node);
            }
            ++i;
        }
        lastNodes = currentNodes;
    }
    return ret;
}

int main() {
    string value = "";
    vector<string> values = {};
    while (getline(cin, value, ',')) {
        values.push_back(value);
    }

    TreeNode* root = createBinTree(values);
    if (root) {
        root->getMaxNum();
    }
    cout << maxSum << endl;
    return 0;
}
```

</details>

### 7-6 计算岛屿最大面积

#### 7-6 题目详情

给你一个大小为 m x n 的二进制矩阵 grid 。

岛屿 是由一些相邻的 1 (代表土地) 构成的组合，这里的「相邻」要求两个 1 必须在 水平或者竖直的四个方向上 相邻。你可以假设 grid 的四个边缘都被 0（代表水）包围着。

岛屿的面积是岛上值为 1 的单元格的数目。

计算并返回 grid 中最大的岛屿面积。如果没有岛屿，则返回面积为 0 。

示例 1：

输入：
grid =
[
    [1, 1, 0, 0, 0];
    [1, 1, 0, 0, 0];
    [0, 0, 1, 0, 0];
    [0, 0, 0, 1, 1]
]

输出：4

示例 2：

输入：grid = [[0,0,0,0,0,0,0,0]]

输出：0

输入格式:
参考下方输入样例，字符串表示的二维数组。例如：

``` text
[[1,1,0,0,0];[1,1,0,0,0];[0,0,1,0,0];[0,0,0,1,1]]
```

输出格式:
数字。例如：4

输入样例:
在这里给出一组输入。例如：

``` text
[[1,1,0,0,0];[1,1,0,0,0];[0,0,1,0,0];[0,0,0,1,1]]
```

输出样例:
在这里给出相应的输出。例如：

``` text
4
```

#### 7-6 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int dfs(vector<vector<int> > & grid, int i, int j) {
    if (i < 0 || i >= (int)grid.size() || j < 0 || j >= (int)grid[0].size() || 0 == grid[i][j]) {
        return 0;
    }
    grid[i][j] = 0;
    return 1 + dfs(grid, i - 1, j) + dfs(grid, i + 1, j) + dfs(grid, i, j - 1) + dfs(grid, i, j + 1);
}

int getMax(vector<vector<int> > & grid) {
    int ans = 0;
    for (int i=0; i < (int)grid.size(); ++i) {
        for (int j=0; j < (int)grid[0].size(); ++j) {
            if (1 == grid[i][j]) {
                ans = max(ans, dfs(grid, i, j));
            }
        }
    }
    return ans;
}

int main() {
    string input = "";
    getline(cin, input);
    replace(input.begin(), input.end(), '[', ' ');
    replace(input.begin(), input.end(), ']', ' ');
    stringstream ssm(input);
    vector<vector<int> > grid;
    string line;
    while (getline(ssm, line, ';')) {
        vector<int> row;
        replace(line.begin(), line.end(), ',', ' ');
        stringstream ssmR(line);
        int num = 0;
        while (ssmR >> num) {
            row.push_back(num);
        }
        grid.push_back(row);
    }
    cout << getMax(grid) << endl;
    return 0;
}
```

</details>

### 7-7 不相交的线

#### 7-7 题目详情

在两条独立的水平线上按给定的顺序写下 nums1 和 nums2 中的整数。

现在，可以绘制一些连接两个数字 nums1[i] 和 nums2[j] 的直线，这些直线需要同时满足满足：

nums1[i] == nums2[j]

且绘制的直线不与任何其他连线（非水平线）相交。

请注意，连线即使在端点也不能相交：每个数字只能属于一条连线。

以这种方法绘制线条，并返回可以绘制的最大连线数。

1 <= nums1.length, nums2.length <= 500

1 <= nums1[i], nums2[j] <= 2000

输入格式:
每组输入为两行，表示nums1和nums2两个数组。每行有n+1个数字，数字间用空格分开，第一个数字表示数组个数n，后面跟n个数字；如2 2 3，表示数组有2个元素，元素值为2和3

输出格式:
输出最多能绘制不想交线的条数。

输入样例:
在这里给出一组输入。例如：

``` text
3 1 4 2
3 1 2 4
6 1 3 7 1 7 5
5 1 9 2 5 1
```

输出样例:
在这里给出相应的输出。例如：

``` text
2
2
```

#### 7-7 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int maxCount(const vector<int>& nums1, const vector<int>& nums2) {
    int m = nums1.size();
    int n = nums2.size();
    int f[m+1][n+1];
    memset(f, 0, sizeof(f));
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (nums1[i-1] == nums2[j-1]) {
                f[i][j] = f[i-1][j-1] + 1;
            } else {
                f[i][j] = max(f[i-1][j], f[i][j-1]);
            }
        }
    }
    return f[m][n];
}

int main() {
    string input = "";
    while (getline(cin, input)) {
        stringstream ssm0(input);
        int m = 0;
        ssm0 >> m;
        vector<int> nums1;
        while (ssm0 >> m) {
            nums1.push_back(m);
        }
        getline(cin, input);
        stringstream ssm1(input);
        ssm1 >> m;
        vector<int> nums2;
        while (ssm1 >> m) {
            nums2.push_back(m);
        }
        cout << maxCount(nums1, nums2) << endl;
    }
    return 0;
}
```

</details>

### 7-8 解码异或后的排列

#### 7-8 题目详情

给你一个整数数组 perm ，它是前 n 个正整数（1,2,3,4,5,…,n-1,n 共n个正整数）的排列，且 n 是个奇数 。

它被加密成另一个长度为 n-1 的整数数组 encoded ，满足 encoded[i] = perm[i] XOR perm[i+1]。比方说，如果 perm=[1,3,2] ，那么 encoded=[2,1]。

给你 encoded 数组，请你返回原始数组 perm 。题目保证答案存在且唯一。

提示：

n 是奇数。

3 <= n < 10^5

encoded.length == n - 1

输入格式:
整数数组encoded，以",”分隔字符串形式作为输入

输出格式:
解码后的原始整数数组perm，以",”分隔字符串形式作为输出

输入样例:
加密后的整数数组encoded：

``` text
6,5,4,6
```

输出样例:
原始数组perm：

``` text
2,4,1,5,3
```

#### 7-8 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

vector<int> getPerms(const vector<int>& encoded) {
    int n = encoded.size() + 1;
    int a = 0, b = 0;
    for (int i=1; i <= n; ++i) {
        a ^= i;
    }
    for (int i=0; i < n-1; i += 2) {
        b ^= encoded[i];
    }
    vector<int> perms(n);
    perms[n - 1] = a ^ b;
    for (int i=n-2; ~i; --i) {
        perms[i] = encoded[i]^perms[i+1];
    }
    return perms;
}

int main() {
    string input = "";
    getline(cin, input);
    replace(input.begin(), input.end(), ',', ' ');
    stringstream ssm(input);
    vector<int> encoded = {};
    int num = 0;
    while (ssm >> num) {
        encoded.push_back(num);
    }
    
    vector<int> perms = getPerms(encoded);

    while (perms.size() > 1) {
        cout << perms.front() << ",";
        perms.erase(perms.begin());
    }
    if (perms.size() > 0) {
         cout << perms.front() << endl;
    }
    
    return 0;
}
```

</details>

### 7-9 最长超赞子字符串

#### 7-9 题目详情

给你一个字符串 s 。请返回 s 中最长的 超赞子字符串 的长度。
「超赞子字符串」需满足满足下述两个条件：
该字符串是 s 的一个非空子字符串
进行任意次数的字符交换后，该字符串可以变成一个回文字符串 1 <= s.length <= 10^5 s 仅由数字组成

输入格式:
输入一行只包含数字的字符串s

输出格式:
输出s中最长的 超赞子字符串 的长度

输入样例:
在这里给出一组输入。例如：

``` text
3242415
```

输出样例:
在这里给出相应的输出。例如：

``` text
5
```

"24241" 是最长的超赞子字符串，交换其中的字符后，可以得到回文 "24142"

#### 7-9 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int longestAwesome(string s) {
    vector<int> d(1024, -1);
    d[0] = 0;
    int st = 0, ans = 1;
    for (int i=1; i <= (int)s.size(); ++i) {
        int v = s[i-1] - '0';
        st ^= 1 << v;
        if (~d[st]) {
            ans = max(ans, i - d[st]);
        } else {
            d[st] = i;
        }
        // 差了一个字符
        for (v=0; v < 10; ++v) {
            if (~d[st^(1 << v)]) {
                ans = max(ans, i - d[st ^ (1 << v)]);
            }
        }
    }
    return ans;
}

int main() {
    string input = "";
    getline(cin, input);
    cout << longestAwesome(input) << endl;
    return 0;
}
```

</details>

### 7-10 超级回文数

#### 7-10 题目详情

如果一个正整数自身是回文数，而且它也是一个回文数的平方，那么我们称这个数为超级回文数。

现在，给定两个正整数 L 和 R ，请按照从小到大的顺序打印包含在范围 [L, R] 中的所有超级回文数。

注：R包含的数字不超过20位

回文数定义：将该数各个位置的数字反转排列，得到的数和原数一样，例如676，2332，10201。

输入格式:
L,R。例如4,1000

输出格式:
[L, R]范围内的超级回文数，例如[4, 9, 121, 484]

输入样例:
在这里给出一组输入。例如：

``` text
4,1000
```

输出样例:
以数组的形式打印符合条件的超级回文数，例如：

``` text
[4, 9, 121, 484]
```

#### 7-10 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

using ll = unsigned long long;

ll ps[100000*2];

int init = []{
    for (int i=1; i < 100000; ++i) {
        string s = to_string(i);
        string t1 = s;
        reverse(t1.begin(), t1.end());
        string t2 = s.substr(0, s.length() - 1);
        reverse(t2.begin(), t2.end());
        ps[2*i-2] = stoll(s + t1);
        ps[2*i-1] = stoll(s + t2);
    }
    sort(begin(ps), end(ps));
    return 0;
}();

bool is_poland(ll x) {
    ll y = 0;
    for (ll t=x; t; t/=10) {
        y = y*10 + t%10;
    }
    return x == y;
}

vector<ll> getNums(const ll& left, const ll& right) {
    vector<ll> nums = {};
    for (ll p : ps) {
        ll x = p * p;
        if (x < left) {
            continue;
        } else if (x >= left && x <= right && is_poland(x)) {
            nums.push_back(x);
        } else if (x > right) {
            break;
        }
    }
    return nums;
}

int main () {
    string input = "";
    getline(cin, input);
    replace(input.begin(), input.end(), ',', ' ');
    stringstream ssm(input);
    ll left;
    ll right;
    ssm >> left >> right;
    
    vector<ll> nums = getNums(left, right);
    cout << "[";
    while (nums.size() > 1) {
        cout << nums.front() << ", ";
        nums.erase(nums.begin());
    }
    if (nums.size() > 0) {
        cout << nums.front();
    }
    cout << "]" << endl;
    return 0;
}
```

</details>

### 7-11 最多能完成排序的块

#### 7-11 题目详情

给你一个整数数组 arr 。将 arr 分割成若干块 ，并将这些块分别进行排序。之后再连接起来，使得连接的结果和按升序排序后的原数组相同。

返回能将数组分成的最多块数？

输入格式:

1. 输入整数数列，元素之间以空格分开
2. 其中数组长度为n，1<=n<=1000,
3. 数组元素 1 <= arr[i], k <= 100，数组元素可有重复整数

输出格式:
数组能分成的最多块数

输入样例1:
在这里给出一组输入。例如：

``` text
5 4 3 2 1
```

输出样例1:
在这里给出相应的输出。例如：

``` text
1
```

解释：

将数组分成2块或者更多块，都无法得到所需的结果。

例如，分成 [5, 4], [3, 2, 1] 的结果是 [4, 5, 1, 2, 3]，这不是有序的数组。

输入样例2:
在这里给出一组输入。例如：

``` text
2 1 3 4 4
```

输出样例2:
在这里给出相应的输出。例如：

``` text
4
```

解释：

可以把它分成两块，例如 [2, 1], [3, 4, 4]。

然而，分成 [2, 1], [3], [4], [4] 可以得到最多的块数。

#### 7-11 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int maxSize(vector<int>& nums) {
    vector<int> sortNums = nums;
    sort(sortNums.begin(), sortNums.end());
    int res = 0;
    int diff = 0;
    for (size_t i=0; i < nums.size(); ++i) {
        if (0 == (diff += (sortNums[i] - nums[i]))) {
            res += 1;
        }
    }
    return res;
}

int main() {
    string input = "";
    getline(cin, input);
    stringstream ssm(input);
    vector<int> nums;
    int num = 0;
    while (ssm >> num) {
        nums.push_back(num);
    }

    cout << maxSize(nums) << endl;
    return 0;
}
```

</details>

### 7-12 最长有效括号

#### 7-12 题目详情

给你一个只包含 '(' 和 ')' 的字符串，找出最长有效（格式正确且连续）括号子串的长度。

输入格式:
包含 '(' 和 ')' 的字符串

输出格式:
有效括号子串的长度

输入样例1:
在这里给出一组输入。例如：

``` text
(()
```

输出样例1:
在这里给出相应的输出。例如：

``` text
2
```

解释：最长有效括号子串是 "()"

输入样例2:
在这里给出一组输入。例如：

``` text
)()(()))
```

输出样例2:
在这里给出相应的输出。例如：

``` text
6
```

解释：最长有效括号子串是 "()(())"

输入样例3:
在这里给出一组输入。例如：

``` text
```

输出样例3:
在这里给出相应的输出。例如：

``` text
0
```

提示：

0 <= s.length <= 3 * 104

s[i] 为 '(' 或 ')'

#### 7-12 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

size_t getMaxLength(const string& input) {
    if (input.length() > 0) {
        stack<size_t> stack;
        stack.push(-1);
        size_t maxLength = 0;

        for (size_t i=0; i < input.length(); ++i) {
            if ('(' == input[i]) {
                // 如果是左括号入栈
                stack.push(i);
            } else {
                // 如是右括号弹出栈顶元素
                stack.pop();
                // 此时
                if (stack.empty()) {
                    // 如是栈为空，将当前索引压入栈
                    stack.push(i);
                } else {
                    // 如歌栈不为空，计算
                    maxLength = max(maxLength, i - stack.top());
                }
            }
        }
        return maxLength;
    }
    return 0;
}

int main() {
    string input = "";
    getline(cin, input);
    cout << getMaxLength(input) << endl;
    return 0;
}
```

</details>

### 7-13 无重复字符的最长子串

#### 7-13 题目详情

给定一个字符串 s ，请你找出其中不含有重复字符的 最长子串 的长度。

输入样例1:
在这里给出一组输入。例如：

``` text
abcabcbb
```

输出样例1:
在这里给出相应的输出。例如：

``` text
3
```

解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。

输入样例2:
在这里给出一组输入。例如：

``` text
bbbbb
```

输出样例2:
在这里给出相应的输出。例如：

``` text
1
```

解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。

输入样例3:
在这里给出一组输入。例如：

pwwkew
输出样例3:
在这里给出相应的输出。例如：

``` text
3
```

解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。

提示：s 由英文字母、数字、符号和空格组成

#### 7-13 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include<bits/stdc++.h>
using namespace std;

int getMaxLengthSubstring(string& input) {
    if (input.length() > 0) {
        unordered_map<char, size_t> charIndexMap;
        size_t start = 0;
        size_t maxLength = 0;
        for (size_t i=0; i < input.length(); ++i) {
            const auto find = charIndexMap.find(input[i]);
            if (find != charIndexMap.end() && find->second >= start) {
                start = find->second + 1;
            }
            charIndexMap[input[i]] = i;
            maxLength = max(maxLength, i - start + 1);
        }
        return maxLength;
    }
    return 0;
}

int main() {
    string input = "";
    getline(cin, input);
    cout << getMaxLengthSubstring(input) << endl;
    return 0;
}

```

</details>

### 7-14 按位与为零的三元组

#### 7-14 题目详情

给你一个整数数组 nums ，返回其中 按位与三元组 的数目。

按位与三元组 是由下标 (i, j, k) 组成的三元组，并满足下述全部条件：

``` text
0 <= i < nums.length
0 <= j < nums.length
0 <= k < nums.length
```

nums[i] & nums[j] & nums[k] == 0 ，其中 & 表示按位与运算符。

提示：

``` text
1 <= nums.length <= 1000
0 <= nums[i] < 2^16
```

注意：时间复杂度是 O(n^3)的算法，会超出时间限制。

输入格式:
整数数组 nums，以",”分隔字符串形式作为输入
输出格式:
一个数字，字符串形式
输入样例:
在这里给出一组输入。例如：

``` text
2,1,3
```

输出样例:
在这里给出相应的输出。例如：

``` text
12
```

#### 7-14 参考答案

<details>
<summary>查看参考代码</summary>

``` cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    string input = "";
    getline(cin, input);

    if (input.length() > 0) {
        vector<int> nums = {};
        replace(input.begin(), input.end(), ',', ' ');
        stringstream ssm(input);
        int num = 0;
        while (ssm >> num) {
            nums.push_back(num);
        }
        
        unordered_map<int, int> count;
        for (int x: nums) {
            for (int y: nums) {
                count[x&y]++;
            }
        }

        int res = 0;
        for (const auto& [k, v] : count) {
            for (int z: nums) {
                if (0 == (k&z)) {
                    res += v;
                }
            }
        }
        cout << res << endl;
    } else {
        cout << 0 << endl;
    }

    return 0;
}
```

</details>
