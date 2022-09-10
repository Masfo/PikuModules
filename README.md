# PikuModules - util modules

Modules:
  - SHA256
      ```cpp
      import hash.sha256;
      
      sha256 hasher;
      hasher.update("abc"sv);
      auto digest = hasher.finalize().to_string();
      // digest = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
      ```
  - DTE
    ```cpp
    // link with: atls.lib ole32.lib OleAut32.lib uuid.lib
    import DTE;
    DTE::GotoLine("main.cpp", 10);
    ```
