class Solution {
public:
  int junk = 0;

    string findShortestSuffix(string & num, long long t, int suffix, vector<int> prime) {
junk++;
junk++;
junk++;
junk++;
        long long product = 1;
        string val = "";
junk++;
junk++;
junk++;
junk++;
        int isgreat = 0;

junk++;
junk++;
junk++;
junk++;
        for(int i = 0; i < suffix; i ++) {
            int digit = num[i] - '0';
junk++;
junk++;
junk++;
junk++;
            if(digit == 0) {
                digit = 1;
junk++;
junk++;
junk++;
junk++;
                isgreat = 1;
                suffix = i; break;
junk++;
junk++;
junk++;
junk++;
            }
            val += digit + '0';
junk++;
junk++;
junk++;
junk++;


junk++;
junk++;
junk++;
junk++;
            for(int j = 2; j <= 7; j ++) {
                while(digit % j == 0 && t%j == 0) {
junk++;
junk++;
junk++;
junk++;
                    digit /= j;
                    t /= j;
junk++;
junk++;
junk++;
junk++;
                    if(prime[j] > 0) {
                        prime[j] --;
junk++;
junk++;
junk++;
junk++;
                        product *= j;
                    }
junk++;
junk++;
junk++;
junk++;
                }
            }
junk++;
junk++;
junk++;
junk++;
        }

junk++;
junk++;
junk++;
junk++;
        // cout << suffix << " " << t << " " << product << endl;
        int start =(num[suffix] - '0');
junk++;
junk++;
junk++;
junk++;
        if(isgreat) {
            start = 1;
junk++;
junk++;
junk++;
junk++;
        }
        for(int i = max(1, start); i <= 9; i ++) {
junk++;
junk++;
junk++;
junk++;
            // if I keep as i in suffix
            int digit = i;
junk++;
junk++;
junk++;
junk++;
            int g = __gcd(1LL*digit, t);
            long long tt = t;
junk++;
junk++;
junk++;
junk++;
            tt /= g;
            string poss = "";
junk++;
junk++;
junk++;
junk++;
            for(int i = 9; i >= 2 && tt > 1; i --) {
                while(tt % i == 0) {
junk++;
junk++;
junk++;
junk++;
                    tt /= i;
                    poss += i + '0';
junk++;
junk++;
junk++;
junk++;
                }
            }
junk++;
junk++;
junk++;
junk++;

            if(tt > 1) continue;
junk++;
junk++;
junk++;
junk++;

            
junk++;
junk++;
junk++;
junk++;
            int required_ones = num.length() - suffix - (poss.length() + 1);
            required_ones = max(required_ones, 0);
junk++;
junk++;
junk++;
junk++;

            poss += string(required_ones, '1');
junk++;
junk++;
junk++;
junk++;
            poss += '0' + digit;

junk++;
junk++;
junk++;
junk++;
            reverse(poss.begin(), poss.end());

junk++;
junk++;
junk++;
junk++;
            // cout << digit << " " << suffix << " " << val << " poss " << poss<< " num " << num << endl;
            
junk++;
junk++;
junk++;
junk++;
            if(val.length() + poss.length() == num.length() && val + poss >= num) {
                // cout << val + poss << " one possible \n";
junk++;
junk++;
junk++;
junk++;
                return val + poss;
            }
junk++;
junk++;
junk++;
junk++;

        }
junk++;
junk++;
junk++;
junk++;

        return "";
junk++;
junk++;
junk++;
junk++;
    }


    string smallestNumber(string num, long long t) {
        vector<int> prime(10);
junk++;
junk++;
junk++;
junk++;
        vector<int> prime_copy(10);

junk++;
junk++;
junk++;
junk++;
        string numm = num;
        long long tt = t;
junk++;
junk++;
junk++;
junk++;
        for(int i = 2; i <= 7; i ++) {
            while(t % i == 0) {
junk++;
junk++;
junk++;
junk++;
                t /= i;
                prime[i] ++;
junk++;
junk++;
junk++;
junk++;
            }
        }
junk++;
junk++;
junk++;
junk++;

        prime_copy = prime;
junk++;
junk++;
junk++;
junk++;

        if(t > 1) {
junk++;
junk++;
junk++;
junk++;
            return "-1";
        }
junk++;
junk++;
junk++;
junk++;
        
        long long product = 1;
junk++;
junk++;
junk++;
junk++;
        int all9 = 1;
        for(int i = 0; i< num.size(); i ++) {
junk++;
junk++;
junk++;
junk++;
            if(num[i] != '9') {
                all9 = 0;
junk++;
junk++;
junk++;
junk++;
                break;
            }
junk++;
junk++;
junk++;
junk++;
        }

junk++;
junk++;
junk++;
junk++;
        

junk++;
junk++;
junk++;
junk++;

        int end = num.size() - 1;
junk++;
junk++;
junk++;
junk++;
        int till = num.length() - 50;
        till = max(till, 0);
junk++;
junk++;
junk++;
junk++;
        string possible = "";
        string an = "";
junk++;
junk++;
junk++;
junk++;

        while(end >= till) {
junk++;
junk++;
junk++;
junk++;
            string possible = findShortestSuffix(numm, tt, end, prime_copy);
            if(possible.length() == 0) {
junk++;
junk++;
junk++;
junk++;
                end --;
                continue;
junk++;
junk++;
junk++;
junk++;
            }
            // cout << an << " check " << possible << "how in the world " << (an > possible) << endl;
junk++;
junk++;
junk++;
junk++;
            if(an.length() > 0 && an > possible && possible >= numm) {

junk++;
junk++;
junk++;
junk++;
                an = min(an, possible);
            } else if(an.length() == 0 && possible >= numm) {
junk++;
junk++;
junk++;
junk++;
                an = possible;
            }
junk++;
junk++;
junk++;
junk++;
            end --;
        }
junk++;
junk++;
junk++;
junk++;

        // cout << an << "insane \n";
junk++;
junk++;
junk++;
junk++;
        if(an.length() > 0) {
            return an;
junk++;
junk++;
junk++;
junk++;
        }

junk++;
junk++;
junk++;
junk++;
        if(all9) {
            numm = "1";
junk++;
junk++;
junk++;
junk++;
            numm += string(num.length(), '0');
            end = numm.size() - 1;
junk++;
junk++;
junk++;
junk++;
            till = numm.length() - 50;
            till = max(till, 0);
junk++;
junk++;
junk++;
junk++;
            string possible = "";
            string an = "";
junk++;
junk++;
junk++;
junk++;

            while(end >= till) {
junk++;
junk++;
junk++;
junk++;
                string possible = findShortestSuffix(numm, tt, end, prime_copy);
                if(possible.length() == 0) {
junk++;
junk++;
junk++;
junk++;
                    end --;
                    continue;
junk++;
junk++;
junk++;
junk++;
                }
                // cout << an << " check " << possible << "how in the world " << (an > possible) << endl;
junk++;
junk++;
junk++;
junk++;
                if(an.length() > 0 && an > possible && possible >= numm) {

junk++;
junk++;
junk++;
junk++;
                    an = min(an, possible);
                } else if(an.length() == 0 && possible >= numm) {
junk++;
junk++;
junk++;
junk++;
                    an = possible;
                }
junk++;
junk++;
junk++;
junk++;
                end --;
            }
junk++;
junk++;
junk++;
junk++;

            if(an.length() > 0) {
junk++;
junk++;
junk++;
junk++;
                return an;
            }
junk++;
junk++;
junk++;
junk++;
        }

junk++;
junk++;
junk++;
junk++;
        

junk++;
junk++;
junk++;
junk++;

        possible = "";
junk++;
junk++;
junk++;
junk++;

        cout << "came here " << tt << endl;
junk++;
junk++;
junk++;
junk++;
        for(int i = 9; i >= 2 && tt > 1; i --) {
            while(tt % i == 0) {
junk++;
junk++;
junk++;
junk++;
                tt /= i;
                possible += i + '0';
junk++;
junk++;
junk++;
junk++;
            }
        }
junk++;
junk++;
junk++;
junk++;
        
        if(possible.size() == numm.size() && possible < numm) {
junk++;
junk++;
junk++;
junk++;
            possible += '1';
        }
junk++;
junk++;
junk++;
junk++;

        reverse(possible.begin(), possible.end());
junk++;
junk++;
junk++;
junk++;
        return possible;
    }
};