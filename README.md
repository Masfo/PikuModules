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
  - **sha256**
      ```cpp
      import hash.sha256;
      
      sha256 hasher;
      hasher.update("abc"sv);
      auto digest = hasher.finalize().to_string();
      // digest = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
      ```
  - **DTE** - *Open file in Visual Studio and goto a line from runtime.*
    ```cpp
    // link with: atls.lib ole32.lib OleAut32.lib uuid.lib
    import DTE;
    DTE::GotoLine("main.cpp", 10);
    ```
