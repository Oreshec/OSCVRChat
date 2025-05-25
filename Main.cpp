#include <iostream>
#include <thread>
#include <chrono>

#include "osc/OscOutboundPacketStream.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"  // путь у вас такой же?

using namespace osc;  // для классов из osc

// Классы из UdpSocket.h — в глобальном namespace (без osc::)

class MyPacketListener : public osc::OscPacketListener {
protected:
    void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint) override {
        try {
            std::cout << "Received message from " << remoteEndpoint.address << ":" << remoteEndpoint.port << std::endl;
            std::cout << "Address: " << m.AddressPattern() << std::endl;
        } catch (...) {
            std::cerr << "Exception parsing message" << std::endl;
        }
    }
};

int main() {
    MyPacketListener listener;

    // Указываем полный путь для IpEndpointName из UdpSocket.h (без osc::)
    IpEndpointName localEndpoint(IpEndpointName::ANY_ADDRESS, 9000);

    // UdpListeningReceiveSocket — в глобальном namespace
    UdpListeningReceiveSocket receiveSocket(localEndpoint, &listener);

    std::thread receiveThread([&receiveSocket]() {
        receiveSocket.Run();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    char buffer[1024];
    OutboundPacketStream p(buffer, sizeof(buffer));

    p << osc::BeginBundleImmediate
      << osc::BeginMessage("/test/address")
      << 42 << 3.14f << "Hello OSC"
      << osc::EndMessage
      << osc::EndBundle;

    // Создаем адрес и сокет для отправки
    IpEndpointName remoteEndpoint("127.0.0.1", 9000);
    UdpTransmitSocket transmitSocket(remoteEndpoint);

    std::cout << "Sending OSC message..." << std::endl;
    transmitSocket.Send(p.Data(), p.Size());

    std::this_thread::sleep_for(std::chrono::seconds(2));

    receiveSocket.AsynchronousBreak();
    receiveThread.join();

    return 0;
}
