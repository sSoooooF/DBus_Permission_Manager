
/*
 * This file was automatically generated by sdbus-c++-xml2cpp; DO NOT EDIT!
 */

#ifndef __sdbuscpp__time_server_glue_h__adaptor__H__
#define __sdbuscpp__time_server_glue_h__adaptor__H__

#include <sdbus-c++/sdbus-c++.h>

#include <string>
#include <tuple>

namespace com
{
namespace system
{

class time_adaptor
{
   public:
    static constexpr const char* INTERFACE_NAME = "com.system.time";

   protected:
    time_adaptor(sdbus::IObject& object) : m_object(object) {}

    time_adaptor(const time_adaptor&) = delete;
    time_adaptor& operator=(const time_adaptor&) = delete;
    time_adaptor(time_adaptor&&) = delete;
    time_adaptor& operator=(time_adaptor&&) = delete;

    ~time_adaptor() = default;

    void registerAdaptor()
    {
        m_object
            .addVTable(sdbus::registerMethod("GetSystemTime")
                           .withOutputParamNames("timestamp")
                           .implementedAs([this]() { return this->GetSystemTime(); }))
            .forInterface(INTERFACE_NAME);
    }

   private:
    virtual uint32_t GetSystemTime() = 0;

   private:
    sdbus::IObject& m_object;
};

}  // namespace system
}  // namespace com

#endif
