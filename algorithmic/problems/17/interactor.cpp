#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

const int MAXN = 100005;
int p[MAXN]; // 全局数组存当前排列

static const int N_SUM_LIMIT = 100000;
static const int SEGMENT_SUM_MULT = 30;

// O(r-l+1): 返回区间 [l,r] 内次大元素的全局位置
int secondLargestPos(int /*n*/, int l, int r) {
    int mxVal = -1, mxPos = -1;
    int seVal = -1, sePos = -1;
    for (int i = l; i <= r; i++) {
        int v = p[i];
        if (v > mxVal) {
            seVal = mxVal; sePos = mxPos;
            mxVal = v;     mxPos = i;
        } else if (v > seVal) {
            seVal = v;     sePos = i;
        }
    }
    return sePos;
}

// 题面给的有界得分（0..100）
double perCaseScoreBounded(int q, int n) {
    double L = log2((double)n);
    if (q <= L) return 100.0;
    if (q >= 15.0 * L) return 0.0;
    return 100.0 * (15.0 * L - (double)q) / (14.0 * L);
}

// 无界比例：按下限为 0 算（不封顶），q=L => 1.0，再少查询可>1，再多查询线性下降到 0
double perCaseRatioUnbounded(int q, int n) {
    double L = log2((double)n);
    if (L <= 0.0) return 0.0;
    double raw = (15.0 * L - (double)q) / (15.0 * L);
    if (raw < 0.0) raw = 0.0; // 下限为 0
    return raw;               // 不设上限
}

int main(int argc, char* argv[]) {
    registerInteraction(argc, argv);

    int T = inf.readInt();
    int nsum = 0;
    cout << T << endl;

    double minScoreBoundedPct = 100.0; // 记录所有测试中的最小有界分（百分制）
    double minRatioUnbounded  = 0.0;   // 记录所有测试中的最小无界比例（下限0、无上限）
    bool firstCase = true;

    int totalQueries = 0;

    for (int tc = 0; tc < T; tc++) {
        int n = inf.readInt();
        nsum += n;
        for (int i = 1; i <= n; i++) {
            p[i] = inf.readInt();
        }

        cout << n << endl;

        // 找到 n 的位置
        int posN = -1;
        for (int i = 1; i <= n; i++) {
            if (p[i] == n) {
                posN = i;
                break;
            }
        }
        if (posN == -1) quitf(_fail, "value n not found (test %d)", tc + 1);

        int segSum = 0, queries = 0;
        int segLimit = SEGMENT_SUM_MULT * n;

        while (true) {
            string op = ouf.readToken();
            if (op == "?") {
                int l = ouf.readInt(1, n, "l");
                int r = ouf.readInt(1, n, "r");
                if (!(l < r)) quitf(_pe, "expected l<r; got l=%d r=%d", l, r);
                int len = r - l + 1;
                if (segSum + len > segLimit) {
                    quitf(_wa, "sum of (r-l+1) exceeds %d (attempt %d) on test %d",
                          segLimit, segSum + len, tc + 1);
                }
                segSum += len;
                queries++;

                int pos = secondLargestPos(n, l, r);
                cout << pos << endl;
            } else if (op == "!") {
                int x = ouf.readInt(1, n, "x");
                if (x != posN) {
                    quitf(_wa, "wrong position on test %d: expected %d, got %d",
                          tc + 1, posN, x);
                }
                // 有界与无界得分/比例
                double boundedPct = perCaseScoreBounded(queries, n);     // 0..100
                double unboundedRatio = perCaseRatioUnbounded(queries, n); // [0, +inf)

                if (firstCase) {
                    minScoreBoundedPct = boundedPct;
                    minRatioUnbounded  = unboundedRatio;
                    firstCase = false;
                } else {
                    minScoreBoundedPct = min(minScoreBoundedPct, boundedPct);
                    minRatioUnbounded  = min(minRatioUnbounded,  unboundedRatio);
                }

                totalQueries += queries;
                break;
            } else {
                quitf(_pe, "unknown command '%s' on test %d", op.c_str(), tc + 1);
            }
        }
    }

    // 最终按最小 case 计分
    double score_ratio = minScoreBoundedPct / 100.0; // [0,1]
    // 无界比例按“下限为0算”，不封顶
    double unbounded_ratio = minRatioUnbounded;      // [0, +inf)

    quitp(score_ratio,
          "all %d tests passed; Ratio: %.4f; RatioUnbounded: %.4f; total queries = %d",
          T, score_ratio, unbounded_ratio, totalQueries);
    return 0;
}
