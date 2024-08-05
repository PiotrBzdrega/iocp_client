#pragma once

#ifdef  __linux__

#elif _WIN32
#include <winsock2.h>
#include <windows.h>
#endif
#include <functional>
#include <string>
#include <thread>
#include <cstdlib>

inline constexpr wchar_t* ms_default_name=L"\\\\.\\mailslot\\ms_instance";
// constexpr wchar_t* change(const char* name)
// {
//     return std::mbstowcs(name,)
// }
constexpr auto READ_BUFFER_SIZE = 2048;

namespace MailSlot
{

     class Sender
     {
     private:
          HANDLE _handle{nullptr};
          HANDLE init(const wchar_t* ms_name);
     public:
          Sender(const wchar_t* ms_name = ms_default_name);
          int write(const std::string &message); //TODO: check, maybe std::string_view will be better
     };

     class Receiver
     {
     private:
          HANDLE _handle{nullptr};
          std::thread read_thr;
          HANDLE init(const wchar_t* ms_name);
          void read(std::function<void(std::string_view)> callback);
          void join();
     public:
          Receiver(std::function<void(std::string_view)> callback, const wchar_t* ms_name = ms_default_name);
          ~Receiver();
     };
}



