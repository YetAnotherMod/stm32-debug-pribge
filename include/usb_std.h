#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stm32f1xx.h>

namespace usb {
constexpr std::uint16_t bcdVersion(uint8_t maj, uint8_t min, uint8_t rev) {
    return (static_cast<uint16_t>(maj) << 8) | ((min & 0xf) << 4) | (rev & 0xf);
}
constexpr std::uint8_t power_ma(uint16_t ma) {
    return std::min(ma / 2 + ma % 2, 250);
}
namespace descriptor {
enum class Type : std::uint8_t {
    device = 0x01,
    configuration = 0x02,
    string = 0x03,
    interface = 0x04,
    endpoint = 0x05,
    qualifier = 0x06,
    other = 0x07,
    interface_power = 0x08,
    otg = 0x09,
    debug = 0x0a,
    interface_assoc = 0x0b,
    cs_interface = 0x24,
    cs_endpoint = 0x25,
};
struct Base {
    std::uint8_t bLength;
    Type bDescriptorType;
    constexpr Base(std::uint8_t bLength, Type bDescriptorType)
        : bLength(bLength), bDescriptorType(bDescriptorType) {}
} __attribute__((packed));
static_assert(sizeof(Base) == 2, "incorrect size");
static_assert(alignof(Base) <= 2, "incorrect align");
struct Device : Base {
    enum class Class : std::uint8_t {
        device = 0x00,
        audio = 0x01,
        comm = 0x02,
        hid = 0x03,
        physical = 0x05,
        image = 0x06,
        printer = 0x07,
        mass_storage = 0x08,
        hub = 0x09,
        cdc_data = 0x0a,
        smart_card = 0x0b,
        content_security = 0x0d,
        video = 0x0e,
        personal_healthcare = 0x0f,
        audio_video = 0x10,
        billboard = 0x11,
        usbc_bridge = 0x12,
        diagnostic = 0xdc,
        wireless_controller = 0xe0,
        misc = 0xef,
        application_specific = 0xfe,
        vendor_specific = 0xff,
    };
    enum class SubClass : std::uint8_t {
        none = 0x00,
        iad = 0x02,
    };
    enum class Protocol : std::uint8_t {
        none = 0x00,
        iad = 0x01,
    };
    uint16_t bcdUSB;
    Class bDeviceClass;
    SubClass bDeviceSubClass;
    Protocol bDeviceProtocol;
    std::uint8_t bMaxPacketSize;
    std::uint16_t idVendor;
    std::uint16_t idProduct;
    std::uint16_t bcdDevice;
    std::uint8_t iManufacturer;
    std::uint8_t iProduct;
    std::uint8_t iSerialNumber;
    std::uint8_t bNumConfigurations;
    constexpr Device(std::uint16_t bcdUSB, Class bDeviceClass,
                     SubClass bDeviceSubClass, Protocol bDeviceProtocol,
                     std::uint8_t bMaxPacketSize, std::uint16_t idVendor,
                     std::uint16_t idProduct, std::uint16_t bcdDevice,
                     std::uint8_t iManufacturer, std::uint8_t iProduct,
                     std::uint8_t iSerialNumber,
                     std::uint8_t bNumConfigurations)
        : Base(sizeof(*this), Type::device), bcdUSB(bcdUSB),
          bDeviceClass(bDeviceClass), bDeviceSubClass(bDeviceSubClass),
          bDeviceProtocol(bDeviceProtocol), bMaxPacketSize(bMaxPacketSize),
          idVendor(idVendor), idProduct(idProduct), bcdDevice(bcdDevice),
          iManufacturer(iManufacturer), iProduct(iProduct),
          iSerialNumber(iSerialNumber), bNumConfigurations(bNumConfigurations) {
    }
} __attribute__((packed));
static_assert(sizeof(Device) == 18, "incorrect size");
static_assert(alignof(Device) <= 2, "incorrect align");
struct Configuration : Base {
    static constexpr std::uint8_t bmAttrRemoteWakeup = 0x20;
    static constexpr std::uint8_t bmAttrSelfPowered = 0x40;
    static constexpr std::uint8_t bmAttrReserved = 0x80;

    std::uint16_t wTotalLength;
    std::uint8_t bNumInterfaces;
    std::uint8_t bConfigurationValue;
    std::uint8_t iConfiguration;
    std::uint8_t bmAttributes;
    std::uint8_t bMaxPower;

    constexpr Configuration(std::uint16_t wTotalLength,
                            std::uint8_t bNumInterfaces,
                            std::uint8_t bConfigurationValue,
                            std::uint8_t iConfiguration,
                            std::uint8_t bmAttributes, std::uint8_t bMaxPower)
        : Base(sizeof(*this), Type::configuration), wTotalLength(wTotalLength),
          bNumInterfaces(bNumInterfaces),
          bConfigurationValue(bConfigurationValue),
          iConfiguration(iConfiguration), bmAttributes(bmAttributes),
          bMaxPower(bMaxPower) {}
} __attribute__((packed));
static_assert(sizeof(Configuration) == 9, "incorrect size");
static_assert(alignof(Configuration) <= 2, "incorrect align");
struct Iad : Base {
    std::uint8_t bFirstInterface;
    std::uint8_t bInterfaceCount;
    std::uint8_t bFunctionClass;
    std::uint8_t bFunctionSubClass;
    std::uint8_t bFunctionProtocol;
    std::uint8_t iFunction;
    constexpr Iad(std::uint8_t bFirstInterface, std::uint8_t bInterfaceCount,
                  std::uint8_t bFunctionClass, std::uint8_t bFunctionSubClass,
                  std::uint8_t bFunctionProtocol, std::uint8_t iFunction)
        : Base(sizeof(*this), Type::interface_assoc),
          bFirstInterface(bFirstInterface), bInterfaceCount(bInterfaceCount),
          bFunctionClass(bFunctionClass), bFunctionSubClass(bFunctionSubClass),
          bFunctionProtocol(bFunctionProtocol), iFunction(iFunction) {}
} __attribute__((packed));
struct Endpoint : Base {
    static constexpr uint8_t directionOut = 0x00;
    static constexpr uint8_t directionIn = 0x80;
    enum class EpType : uint8_t {
        control = 0x00,
        isochronous = 0x01,
        bulk = 0x02,
        interrupt = 0x03
    };
    std::uint8_t bEndpointAddress;
    EpType bmAttributes;
    std::uint16_t wMaxPacketSize;
    std::uint8_t bInterval;
    constexpr Endpoint(std::uint8_t bEndpointAddress, EpType bmAttributes,
                       std::uint16_t wMaxPacketSize, std::uint8_t bInterval)
        : Base(sizeof(*this), Type::endpoint),
          bEndpointAddress(bEndpointAddress), bmAttributes(bmAttributes),
          wMaxPacketSize(wMaxPacketSize), bInterval(bInterval) {}
} __attribute__((packed));

template <char16_t... chars> struct String : Base {
    char16_t wString[sizeof...(chars)];
    constexpr String() : Base(sizeof(*this), Type::string), wString{chars...} {}
};

template <typename CharT, CharT... chars> String<chars...> operator"" _sd() {
    static_assert(sizeof(CharT) == 2, "must be utf16 string");
    return String<chars...>();
}

static_assert(sizeof(u"abc"_sd) == 8, "bad size");

struct Debug : Base {
    uint8_t bDebugInEndpoint;
    uint8_t bDebugOutEndpoint;
    constexpr Debug(uint8_t bDebugInEndpoint, uint8_t bDebugOutEndpoint)
        : Base(sizeof(*this), Type::debug), bDebugInEndpoint(bDebugInEndpoint),
          bDebugOutEndpoint(bDebugOutEndpoint) {}
};

} // namespace descriptor

struct Setup {
    static constexpr std::uint8_t direction_host_to_device = 0;
    static constexpr std::uint8_t direction_device_to_host = 1;
    static constexpr std::uint8_t type_standard = 0;
    static constexpr std::uint8_t type_class_ = 1;
    static constexpr std::uint8_t type_vendor = 2;
    static constexpr std::uint8_t type_reserved = 3;
    static constexpr std::uint8_t recipient_device = 0;
    static constexpr std::uint8_t recipient_interface = 1;
    static constexpr std::uint8_t recipient_endpoint = 2;
    static constexpr std::uint8_t recipient_other = 3;

    union {
        std::uint8_t bmRequestType;
        struct {
            std::uint8_t recipient : 5;
            std::uint8_t type : 2;
            std::uint8_t direction : 1;
        };
    };
    enum class Request : std::uint8_t {
        get_status = 0x00,
        clear_feature = 0x01,
        set_feature = 0x03,
        set_address = 0x05,
        get_descriptor = 0x06,
        set_descriptor = 0x07,
        get_configuration = 0x08,
        set_configuration = 0x09
    };
    Request bRequest;
    std::uint16_t wValue;
    std::uint16_t wIndex;
    std::uint16_t wLength;
    uint8_t payload[256 - 8];
} __attribute__((packed));

static_assert(sizeof(Setup) == 256, "bad size");
static_assert(alignof(Setup) == 1, "bad align");

namespace io {

using pbAlignedWord = std::uint16_t __attribute__((aligned(4)));
static_assert(sizeof(pbAlignedWord) == 2);
static_assert(alignof(pbAlignedWord) == 4);

namespace bTable {

struct bTableEntity {
    pbAlignedWord txOffset;
    pbAlignedWord txCount;
    pbAlignedWord rxOffset;
    pbAlignedWord rxCount;
};

static_assert(sizeof(bTableEntity) == 16);
static_assert(alignof(bTableEntity) == 4);

struct pBufferData {
    pbAlignedWord data;
};

static_assert(sizeof(pBufferData) == 4);
static_assert(alignof(pBufferData) == 4);

constexpr size_t smallBlockSize = 2;
constexpr size_t largeBlockSize = 32;
constexpr size_t smallBlockSizeLimit =
    ((USB_COUNT0_RX_NUM_BLOCK_Msk >> USB_COUNT0_RX_NUM_BLOCK_Pos) << 1);

} // namespace bTable

volatile uint16_t &epRegs(uint16_t num) { return (&USB->EP0R)[num * 2]; }

using endpointCallback = void (*)(unsigned int ep_num);

struct Endpoint {
    uint32_t type;
    uint8_t rxSize;
    uint8_t txSize;
    endpointCallback rxHandler;
    endpointCallback txHandler;
    endpointCallback setupHandler;
};

} // namespace io

enum class status{
    ack = 0x00,
    nak = 0x01,
    fail = 0x02
};

enum class deviceState {
    usb_device_state_reset          = 0x00,
    usb_device_state_address_set    = 0x01,
    usb_device_state_configured     = 0x02
};

} // namespace usb
