#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <algorithm>
#include <functional>

using namespace std;


struct EndRef {
    int val;
    explicit EndRef(int v = 0) : val(v) {}
};

struct Node {
    unordered_map<char, int> next;

    int suf = -1;    
    int start = -1;  
    EndRef* end = nullptr;
    int parent = -1; 
    int id = -1;   
};

struct GST {
    string S;          
    vector<Node> t;  
    int root;         
    EndRef rootEnd;     
    EndRef right;     
    int n; 

    int active_node;
    int active_edge;
    int active_len;

    int remaining;
    int last_new;

    vector<int> depth;
    vector<int> mark;   

    GST(const string& s)
        : S(s), rootEnd(-1), right(0), n((int)s.size()),
          active_node(0), active_edge(-1), active_len(0),
          remaining(0), last_new(-1) {
        t.reserve(2 * n + 10);
        root = new_node(-1, &rootEnd, -1);
        t[root].suf = root; 
        active_node = root;
    }

    int new_node(int start, EndRef* end, int parent) {
        int id = (int)t.size();
        t.push_back(Node{});
        t.back().start = start;
        t.back().end = end;
        t.back().parent = parent;
        t.back().id = id;
        return id;
    }

    static int edge_len(const Node& v) {
        return v.end->val - v.start;
    }

    bool walk_down(int nxt) {
        int elen = edge_len(t[nxt]);
        if (active_len >= elen) {
            active_edge += elen;
            active_len -= elen;
            active_node = nxt;
            return true;
        }
        return false;
    }

    void build() {
        for (int pos = 0; pos < n; ++pos) {
            extend(pos);
        }
    }

    void extend(int pos) {
        right.val = pos + 1;

        last_new = -1;
        ++remaining;
        while (remaining > 0) {
            if (active_len == 0) active_edge = pos;

            char c = S[active_edge];
            if (t[active_node].next.find(c) == t[active_node].next.end()) {
                int leaf = new_node(pos, &right, active_node);
                t[active_node].next[c] = leaf;

                if (last_new != -1) {
                    t[last_new].suf = active_node;
                    last_new = -1;
                }
            } else {
                int nxt = t[active_node].next[c];
                if (walk_down(nxt)) continue;

                int edge_pos = t[nxt].start + active_len;
                if (S[edge_pos] == S[pos]) {
                    ++active_len;
                    if (last_new != -1) {
                        t[last_new].suf = active_node;
                        last_new = -1;
                    }
                    break;
                }

                int split = new_node(t[nxt].start, new EndRef(t[nxt].start + active_len), active_node);
                t[active_node].next[c] = split;

                int leaf = new_node(pos, &right, split);
                t[split].next[S[pos]] = leaf;

                t[nxt].start += active_len;
                t[nxt].parent = split;
                t[split].next[S[t[nxt].start]] = nxt;

                if (last_new != -1) {
                    t[last_new].suf = split;
                }
                last_new = split;
            }

            --remaining;

            if (active_node == root && active_len > 0) {
                --active_len;
                active_edge = pos - remaining + 1;
            } else {
                active_node = (t[active_node].suf == -1) ? root : t[active_node].suf;
            }
        }
    }

    void compute_depths() {
        depth.assign(t.size(), 0);
        vector<int> st;
        st.push_back(root);
        vector<int> it; it.push_back(0);
        vector<vector<pair<char,int>>> kids(t.size());

        for (int v = 0; v < (int)t.size(); ++v) {
            kids[v].reserve(t[v].next.size());
            for (auto &kv : t[v].next) kids[v].push_back(kv);
        }

        while (!st.empty()) {
            int v = st.back();
            int &i = it.back();
            if (i == 0) {
                if (v == root) depth[v] = 0;
                else           depth[v] = depth[t[v].parent] + edge_len(t[v]);
            }
            if (i < (int)kids[v].size()) {
                int u = kids[v][i++].second;
                st.push_back(u);
                it.push_back(0);
            } else {
                st.pop_back();
                it.pop_back();
            }
        }
    }

    int dfs_mark(int v, int sep1_idx) {
        if (t[v].next.empty()) {
            int start_pos = t[v].end->val - depth[v];

            if (start_pos < sep1_idx)       return mark[v] = 1;
            else if (start_pos > sep1_idx)  return mark[v] = 2; 
            else                            return mark[v] = 0;
        }
        int m = 0;
        for (auto &kv : t[v].next) {
            m |= dfs_mark(kv.second, sep1_idx);
        }
        return mark[v] = m;
    }

    void collect_all_L_of_mark3(int L, set<string>& out) {
        if (L == 0) return;

        string current;
        current.reserve(L);

        function<void(int,int)> go = [&](int v, int built) {
            if (mark[v] != 3) return;
            if (built == L) {
                out.insert(current);
                return;
            }
            for (auto &kv : t[v].next) {
                int u = kv.second;
                if (mark[u] != 3) continue;

                int take = min(L - built, edge_len(t[u]));
                current.append(S.begin() + t[u].start, S.begin() + t[u].start + take);

                if (built + edge_len(t[u]) <= L) {
                    go(u, built + edge_len(t[u]));
                } else {
                    out.insert(current);
                }
                current.resize(current.size() - take);
            }
        };

        go(root, 0);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s1, s2;
    if (!getline(cin, s1)) return 0;
    if (!getline(cin, s2)) return 0;

    string S = s1 + '#' + s2 + '$';
    int sep1_idx = (int)s1.size();

    GST gst(S);
    gst.build();
    gst.compute_depths();
    gst.mark.assign(gst.t.size(), 0);

    gst.dfs_mark(gst.root, sep1_idx);

    int L = 0;
    for (const auto& v : gst.t) {
        if (gst.mark[v.id] == 3) {
            L = max(L, gst.depth[v.id]);
        }
    }

    cout << L << "\n";
    if (L == 0) {
        return 0;
    }

    set<string> answers;
    gst.collect_all_L_of_mark3(L, answers);
    for (auto &x : answers) {
        cout << x << "\n";
    }
    return 0;
}
