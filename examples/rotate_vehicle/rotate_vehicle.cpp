#include <iostream> 
#include <thread> 
#include <chrono>
#include <future>
#include <mavsdk/mavsdk.h> 
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

using namespace mavsdk;
using namespace std;

// 
bool offboard_ctrl_body(std::shared_ptr<mavsdk::Offboard> offboard){
    Offboard::Attitude control_stick{}; // Send once before starting offboard
    offboard->set_attitude(control_stick);
    Offboard::Result offboard_result = offboard->start();
    if (offboard_result != Offboard::Result::Success) { return 1; } // Failed
    control_stick.roll_deg = 30.0f;
    control_stick.thrust_value = 0.6f;
    offboard->set_attitude(control_stick);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    offboard_result = offboard->stop(); // Send it once before ending offboard
    return true;
}


int main(int argc,char ** argv)
{
    Mavsdk mavsdk;
    ConnectionResult connection_result;
    bool discovered_system = false;
    connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result!= ConnectionResult::Success){return 1;}
        mavsdk.subscribe_on_new_system([&mavsdk,&discovered_system](){
        const auto system = mavsdk.systems().at(0);
        if(system->is_connected()){discovered_system=true;}
    });
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if(!discovered_system){return 1;}
    const auto system = mavsdk.systems().at(0);
    system->register_component_discovered_callback(
        [](ComponentType common_type)->void{std::cout << unsigned(common_type);}
    );
    //regist_telemetry.cpp 
    auto telemetry = std::make_shared<Telemetry>(system);
    const Telemetry::Result set_rate_result = telemetry->set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::Success) { return 1; }
    telemetry->subscribe_position([](Telemetry::Position position) {
        std::cout << "Altitude: " << position.relative_altitude_m << " m" << std::endl;
    }); // Set up callback to monitor altitude
    while (telemetry->health_all_ok() != true) {
        std::cout << "Vehicle is getting ready to arm" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } // Check if vehicle is ready to arm
    // arm.cpp
    auto action = std::make_shared<Action>(system);
    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action->arm();

    if (arm_result != Action::Result::Success) {
    std::cout << "Arming failed:" << arm_result << std::endl;
    return 1;
    }
    // takeoff.cpp
    std::cout << "Taking off..." << std::endl;
    const Action::Result takeoff_result = action->takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cout << "Takeoff failed:" << takeoff_result << std::endl;
        return 1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    // rotate_vehicle.cpp / insert
    auto offboard = std::make_shared<Offboard>(system);
    bool ret = offboard_ctrl_body(offboard); 
    if (ret == false) { return -1; }
    // 
    return 0;
}

