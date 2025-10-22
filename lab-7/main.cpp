#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int32_t n, m;
    if (!(std::cin >> n >> m)) return 0;

    std::vector<int32_t> h(m, 0);

    std::string s;
    s.reserve(static_cast<std::size_t>(m));

    std::vector<int32_t> st;
    st.reserve(static_cast<std::size_t>(m + 1));

    int64_t best = 0;
    for (int32_t i = 0; i < n; ++i) {
        std::cin >> s;

        for (int32_t j = 0; j < m; ++j) {
            h[j] = (s[j] == '0') ? (h[j] + 1) : 0;
        }

        st.clear();
        for (int32_t j = 0; j <= m; ++j) {
            int32_t cur = (j == m ? 0 : h[j]);

            while (!st.empty() && h[st.back()] > cur) {
                int32_t idx = st.back(); st.pop_back();
                int32_t height = h[idx];
                int32_t left = st.empty() ? 0 : (st.back() + 1);
                int32_t width = j - left;
                best = std::max(best, static_cast<int64_t>(height) * width);
            }
            st.push_back(j);
        }
    }

    std::cout << best << '\n';
    return 0;
}
