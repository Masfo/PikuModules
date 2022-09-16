# PikuModules - util modules

Modules:
  - **piku.debug**
    ```cpp
    import piku.debug;
    
    piku::trace("Hello {}", "World"); // winmain.cpp(15): Hello World

    piku::println("Hi, World"); // Hi, World
    piku::print("no newline");

    piku::assert_msg(true, "Message");
    piku::assert(true);

    ```
  - **SHA2**
      ```cpp
      import hash.sha2;
            
      sha256 h256;
      h256.update("abc"sv);
      auto digest256 = h256.finalize().to_string();
      // digest256 = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad


      sha512 h512;
      h512.update("abc"sv);
      auto digest512 = h512.finalize().to_string();
      // digest512 = ddaf35a193617abacc417349ae204......d4423643ce80e2a9ac94fa54ca49f
      ```
  - **DTE** - *Open file in Visual Studio and goto a line from runtime.*
    ```cpp
    // link with: atls.lib ole32.lib OleAut32.lib uuid.lib
    import DTE;
    DTE::GotoLine("main.cpp", 10);
    ```
  - **Types**
      ```cpp
      import piku.types;

      // i8,u8,i16,u16,i32,u32,i64,u64

      u8 u  = 64_u8;
      i64 i = 112233_i64;

      auto value = as<uint32>(256);

      ```
