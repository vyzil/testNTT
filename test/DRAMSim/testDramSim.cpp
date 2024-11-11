#include <iostream>
#include <vector>
#include <utility>
#include "dramsim3.h"

int main() {
    std::vector<std::pair<uint64_t, uint64_t>> completion_times; // (Address, Cycle)
    // std::vector<std::pair<uint64_t, double>> completion_times; // (Address, tck)
    uint64_t current_clock = 0;

    auto dramsim = dramsim3::GetMemorySystem(
        "../configs/DDR4_4Gb_x16_2666.ini",   // Config File Path
        // "../configs/HBM2_8Gb_x128.ini",
        ".",                                   // Output File Path
        [&completion_times, &current_clock](uint64_t addr) { // Read Callback
            std::cout << "Read callback at address: " << addr << " at clock: " << current_clock << std::endl;
            completion_times.emplace_back(addr, current_clock);
        },                                    
        [](uint64_t addr) { 
            std::cout << "Write callback at address: " << addr << std::endl;
        }                                     // Write Callback
    );

    // 테스트용 메모리 주소들
    uint64_t addresses[] = {0x2000D5C0, 0x2000D600, 0x2000D640, 0x2000D600, 0x2000D640, 0x2000D600};
    bool is_write = false; // 읽기 요청으로 설정

    // 메모리 접근 요청 추가
    for (size_t i = 0; i < sizeof(addresses) / sizeof(addresses[0]); ++i) {
        dramsim->AddTransaction(addresses[i], is_write);
    }

    // 클럭 틱 진행 - 요청이 완료될 때까지 진행
    while (completion_times.size() < sizeof(addresses) / sizeof(addresses[0])) {
        dramsim->ClockTick(); // 메모리 요청 처리 시뮬레이션
        current_clock++;      // 현재 클럭 주기 증가
        printf("%f\n", dramsim->GetTCK());
    }

    // 요청별 완료 시간 출력
    for (size_t i = 0; i < sizeof(addresses) / sizeof(addresses[0]); ++i) {
        std::cout << "Address " << addresses[i] << " completed at clock: " 
                  << completion_times[i].second << std::endl;
    }

    std::cout << "All requests completed at clock: " << current_clock << std::endl;

    return 0;
}
