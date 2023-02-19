#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

int main() {
    vector<pair<size_t, size_t>> lights;
    vector<size_t> flags;

    int speed;
    cin >> speed; cin.ignore();
    int lightCount;
    cin >> lightCount; cin.ignore();

    for (int i = 0; i < lightCount; i++) {
        int distance, duration;
        cin >> distance >> duration; cin.ignore();
        lights.push_back(pair<size_t, size_t>(distance, duration));
    }

    while (speed) {
        flags.clear();
        for (int i = 0; i < lightCount; i++)
            flags.push_back(0);

        double error = 0.0003;
        double speed_meters_per_second = static_cast<double>(speed) * 1000.0 / 3600.0;

        for (int i = 0; i < lightCount; i++) {
            int distance = lights[i].first;
            int duration = lights[i].second;
            double amount_of_switches_d = distance / speed_meters_per_second / duration;
            size_t amount_of_switches = static_cast<size_t>(floor(amount_of_switches_d + error));
            
            if (amount_of_switches % 2 == 0)
                flags[i] = 1;
            else
                break;
        }

        int result = 1;
        for (auto flag : flags) result *= flag;

        if (result) break;
        speed--;
    }

    cout << speed;
}
