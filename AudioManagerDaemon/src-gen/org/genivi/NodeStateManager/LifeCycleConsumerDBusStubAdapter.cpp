/*
* This file was generated by the CommonAPI Generators.
* Used org.genivi.commonapi.core 2.1.5.201312121915.
* Used org.franca.core 0.8.11.201401091023.
*
* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
* If a copy of the MPL was not distributed with this file, You can obtain one at
* http://mozilla.org/MPL/2.0/.
*/
#include "LifeCycleConsumerDBusStubAdapter.h"
#include <org/genivi/NodeStateManager/LifeCycleConsumer.h>

namespace org {
namespace genivi {
namespace NodeStateManager {

std::shared_ptr<CommonAPI::DBus::DBusStubAdapter> createLifeCycleConsumerDBusStubAdapter(
                   const std::shared_ptr<CommonAPI::DBus::DBusFactory>& factory,
                   const std::string& commonApiAddress,
                   const std::string& interfaceName,
                   const std::string& busName,
                   const std::string& objectPath,
                   const std::shared_ptr<CommonAPI::DBus::DBusProxyConnection>& dbusProxyConnection,
                   const std::shared_ptr<CommonAPI::StubBase>& stubBase) {
    return std::make_shared<LifeCycleConsumerDBusStubAdapter>(factory, commonApiAddress, interfaceName, busName, objectPath, dbusProxyConnection, stubBase);
}

__attribute__((constructor)) void registerLifeCycleConsumerDBusStubAdapter(void) {
    CommonAPI::DBus::DBusFactory::registerAdapterFactoryMethod(LifeCycleConsumer::getInterfaceId(),
                                                               &createLifeCycleConsumerDBusStubAdapter);
}



LifeCycleConsumerDBusStubAdapterInternal::~LifeCycleConsumerDBusStubAdapterInternal() {
    deactivateManagedInstances();
    LifeCycleConsumerDBusStubAdapterHelper::deinit();
}

void LifeCycleConsumerDBusStubAdapterInternal::deactivateManagedInstances() {
}

const char* LifeCycleConsumerDBusStubAdapterInternal::getMethodsDBusIntrospectionXmlData() const {
    static const std::string introspectionData =
        "<method name=\"getInterfaceVersion\">\n"
            "<arg name=\"value\" type=\"uu\" direction=\"out\" />"
        "</method>\n"
        "<method name=\"LifecycleRequest\">\n"
            "<arg name=\"Request\" type=\"u\" direction=\"in\" />\n"
            "<arg name=\"RequestId\" type=\"u\" direction=\"in\" />\n"
            "<arg name=\"ErrorCode\" type=\"i\" direction=\"out\" />\n"
        "</method>\n"

    ;
    return introspectionData.c_str();
}

static CommonAPI::DBus::DBusGetAttributeStubDispatcher<
        LifeCycleConsumerStub,
        CommonAPI::Version
        > getLifeCycleConsumerInterfaceVersionStubDispatcher(&LifeCycleConsumerStub::getInterfaceVersion, "uu");


static CommonAPI::DBus::DBusMethodWithReplyStubDispatcher<
    LifeCycleConsumerStub,
    std::tuple<uint32_t, uint32_t>,
    std::tuple<int32_t>
    > lifecycleRequestStubDispatcher(&LifeCycleConsumerStub::LifecycleRequest, "i");



const LifeCycleConsumerDBusStubAdapterHelper::StubDispatcherTable& LifeCycleConsumerDBusStubAdapterInternal::getStubDispatcherTable() {
    return stubDispatcherTable_;
}


LifeCycleConsumerDBusStubAdapterInternal::LifeCycleConsumerDBusStubAdapterInternal(
        const std::shared_ptr<CommonAPI::DBus::DBusFactory>& factory,
        const std::string& commonApiAddress,
        const std::string& dbusInterfaceName,
        const std::string& dbusBusName,
        const std::string& dbusObjectPath,
        const std::shared_ptr<CommonAPI::DBus::DBusProxyConnection>& dbusConnection,
        const std::shared_ptr<CommonAPI::StubBase>& stub):
        CommonAPI::DBus::DBusStubAdapter(
                factory,
                commonApiAddress,
                dbusInterfaceName,
                dbusBusName,
                dbusObjectPath,
                dbusConnection,
                false),
        LifeCycleConsumerDBusStubAdapterHelper(
            factory,
            commonApiAddress,
            dbusInterfaceName,
            dbusBusName,
            dbusObjectPath,
            dbusConnection,
            std::dynamic_pointer_cast<LifeCycleConsumerStub>(stub),
            false),
        stubDispatcherTable_({
            { { "LifecycleRequest", "uu" }, &org::genivi::NodeStateManager::lifecycleRequestStubDispatcher }
            }) {

    stubDispatcherTable_.insert({ { "getInterfaceVersion", "" }, &org::genivi::NodeStateManager::getLifeCycleConsumerInterfaceVersionStubDispatcher });
}

} // namespace NodeStateManager
} // namespace genivi
} // namespace org