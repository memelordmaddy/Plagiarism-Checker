class Solution {
private:
    long long power(long long n, long long expo, long long mod)
    {
        long long ans = 1;

        while (expo)
        {
            if (expo % 2)
                ans = ans * n % mod;

            n = n * n % mod;
            expo /= 2;
        }

        return ans;
    }

    long long getNumBits(long long n)
    {
        if (n == 0)
            return 0;

        long long ans = 0, num = n + 1;

        for (int i = 60; i >= 0; i--)
            if ((1LL << i) < num)
            {
                ans = getNumBits((1LL << i) - 1);

                long long rem = num - (1LL << i);

                ans += getNumBits(rem - 1) + rem;
                break;
            }

        return ans;
    }

    long long getMaxNum(long long n)
    {
        long long left = 1, right = 1e15, ans = 0;

        while (left <= right)
        {
            long long mid = (left + right) / 2;

            long long placesOccupied = getNumBits(mid);

            if (placesOccupied <= n)
                ans = mid, left = mid + 1;
            else
                right = mid - 1;
        }

        return ans;
    }

    long long getSum(long long n)
    {
        long long res = 0;

        for (int i = 1; i < 50; i++)
        {
            long long blockSize = (1LL << (i + 1));
            long long onesInBlock = blockSize / 2;

            long long completeBlocks = n / blockSize;

            res += completeBlocks * onesInBlock * i;

            long long rem = n % blockSize;
            res += max(0LL, rem + 1 - onesInBlock) * i;
        }

        return res;
    }

    long long func(long long n)
    {
        if (n == 0)
            return 0;

        long long maxNum = getMaxNum(n);
        long long placesOccupied = getNumBits(maxNum);

        long long ans = getSum(maxNum);

        maxNum++;
        long long rem = n - placesOccupied;

        for (int i = 0; rem > 0; i++)
            if (maxNum & (1LL << i))
            {
                rem--;
                ans += i;
            }

        return ans;
    }

    int solve(vector<long long> &query) {
        long long L = query[0], R = query[1], mod = query[2];

        return power(2, func(R + 1) - func(L), mod) % mod;
    }
public:
    vector<int> findProductsOfElements(vector<vector<long long>>& queries) {
        vector<int> res;

        for (auto q : queries)
            res.push_back(solve(q));

        return res;
    }
};