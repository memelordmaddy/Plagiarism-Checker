class Solution {
public:

    string findShortestSuffix(string & num, long long t, long long int suffix, vector<long long int> prime) {
        long long product = 1;
        string val = "";
        long long int isgreat = 0;

        for(long long int i = 0; i < suffix; i ++) {
            long long int digit = num[i] - '0';
            if(digit == 0) {
                digit = 1;
                isgreat = 1;
                suffix = i; break;
            }
            val += digit + '0';


            for(long long int j = 2; j <= 7; j ++) {
                while(digit % j == 0 && t%j == 0) {
                    digit /= j;
                    t /= j;
                    if(prime[j] > 0) {
                        prime[j] --;
                        product *= j;
                    }
                }
            }
        }

        // cout << suffix << " " << t << " " << product << endl;
        long long int start =(num[suffix] - '0');
        if(isgreat) {
            start = 1;
        }
        for(long long int i = max(1, start); i <= 9; i ++) {
            // if I keep as i in suffix
            long long int digit = i;
            long long int g = __gcd(1LL*digit, t);
            long long tt = t;
            tt /= g;
            string poss = "";
            for(long long int i = 9; i >= 2 && tt > 1; i --) {
                while(tt % i == 0) {
                    tt /= i;
                    poss += i + '0';
                }
            }

            if(tt > 1) continue;

            
            long long int required_ones = num.length() - suffix - (poss.length() + 1);
            required_ones = max(required_ones, 0);

            poss += string(required_ones, '1');
            poss += '0' + digit;

            reverse(poss.begin(), poss.end());

            // cout << digit << " " << suffix << " " << val << " poss " << poss<< " num " << num << endl;
            
            if(val.length() + poss.length() == num.length() && val + poss >= num) {
                // cout << val + poss << " one possible \n";
                return val + poss;
            }

        }

        return "";
    }

    string smallestNumber(string num, long long t) {
        vector<long long int> prime(10);
        vector<long long int> prime_copy(10);

        string numm = num;
        long long tt = t;
        for(long long int i = 2; i <= 7; i ++) {
            while(t % i == 0) {
                t /= i;
                prime[i] ++;
            }
        }

        prime_copy = prime;

        if(t > 1) {
            return "-1";
        }
        
        long long product = 1;
        long long int all9 = 1;
        for(long long int i = 0; i< num.size(); i ++) {
            if(num[i] != '9') {
                all9 = 0;
                break;
            }
        }

        


        long long int end = num.size() - 1;
        long long int till = num.length() - 50;
        till = max(till, 0);
        string possible = "";
        string an = "";

        while(end >= till) {
            string possible = findShortestSuffix(numm, tt, end, prime_copy);
            if(possible.length() == 0) {
                end --;
                continue;
            }
            // cout << an << " check " << possible << "how in the world " << (an > possible) << endl;
            if(an.length() > 0 && an > possible && possible >= numm) {

                an = min(an, possible);
            } else if(an.length() == 0 && possible >= numm) {
                an = possible;
            }
            end --;
        }

        // cout << an << "insane \n";
        if(an.length() > 0) {
            return an;
        }

        if(all9) {
            numm = "1";
            numm += string(num.length(), '0');
            end = numm.size() - 1;
            till = numm.length() - 50;
            till = max(till, 0);
            string possible = "";
            string an = "";

            while(end >= till) {
                string possible = findShortestSuffix(numm, tt, end, prime_copy);
                if(possible.length() == 0) {
                    end --;
                    continue;
                }
                // cout << an << " check " << possible << "how in the world " << (an > possible) << endl;
                if(an.length() > 0 && an > possible && possible >= numm) {

                    an = min(an, possible);
                } else if(an.length() == 0 && possible >= numm) {
                    an = possible;
                }
                end --;
            }

            if(an.length() > 0) {
                return an;
            }
        }

        


        possible = "";

        cout << "came here " << tt << endl;
        for(long long int i = 9; i >= 2 && tt > 1; i --) {
            while(tt % i == 0) {
                tt /= i;
                possible += i + '0';
            }
        }
        
        if(possible.size() == numm.size() && possible < numm) {
            possible += '1';
        }

        reverse(possible.begin(), possible.end());
        return possible;
    }
};