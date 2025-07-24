# 🖥️ Silver Clinic - Instalação Windows 10+

## 📋 Pré-requisitos

### 1. **Visual Studio 2019/2022**
- Download: [Visual Studio Community](https://visualstudio.microsoft.com/pt-br/downloads/)
- Componentes necessários:
  - C++ CMake tools
  - MSVC v143 compiler toolset
  - Windows 10/11 SDK

### 2. **CMake** (versão 3.16+)
- Download: [CMake](https://cmake.org/download/)
- Adicionar ao PATH durante instalação

### 3. **PowerShell** (Recomendado)
- **PowerShell 7+** (pwsh): [Download PowerShell](https://github.com/PowerShell/PowerShell/releases)
- **OU** Windows PowerShell 5.1 (já incluído no Windows 10+)
- Verificar: `pwsh --version` ou `powershell --version`

### 4. **Git**
- Download: [Git for Windows](https://git-scm.com/download/win)

## 🚀 Instalação Automática (Recomendada)

### Opção 1: vcpkg (Mais Fácil)
```cmd
# 1. Instalar vcpkg
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# 2. Instalar dependências
.\vcpkg install sqlite3:x64-windows
.\vcpkg install libharu:x64-windows

# 3. Adicionar ao PATH (Variáveis de Ambiente)
set VCPKG_ROOT=C:\vcpkg

# 4. Clonar e compilar projeto
git clone <repository-url> SilverClinic
cd SilverClinic
.\build_windows.bat
```

### Opção 2: PowerShell (Recomendado)
```powershell
# Execute PowerShell como Administrador (opcional para vcpkg)
# Use 'pwsh' se tiver PowerShell 7+, ou 'powershell' para Windows PowerShell

# Clone e compile
git clone <repository-url> SilverClinic
cd SilverClinic

# Build completo com opções
.\build_windows.ps1 -Configuration Release

# OU build rápido
.\build_windows.ps1

# Executar aplicação
.\run_app.ps1

# Executar todos os testes
.\run_tests_windows.ps1
```

## 🔧 Instalação Manual

### 1. **SQLite3**
- Download: [SQLite3 Precompiled](https://www.sqlite.org/download.html)
- Extrair para `libs/` no projeto:
  ```
  SilverClinic/
  └── libs/
      ├── sqlite3.h
      ├── sqlite3.lib
      └── sqlite3.dll
  ```

### 2. **libharu (PDF)**
- Download: [libharu releases](https://github.com/libharu/libharu/releases)
- Compilar ou usar binários pré-compilados
- Extrair para `libs/`:
  ```
  SilverClinic/
  └── libs/
      ├── hpdf.h
      └── libhpdf.lib
  ```

### 3. **Compilar Manualmente**
```cmd
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

## 📂 Estrutura de Arquivos Windows

```
SilverClinic/
├── build/
│   └── Release/
│       ├── SilverClinic.exe          ← Aplicação principal
│       ├── sqlite3.dll               ← DLL SQLite (se necessário)
│       └── libhpdf.dll               ← DLL libharu (se necessário)
├── data/
│   ├── clinic.db                     ← Banco de dados
│   └── pdfs/                         ← PDFs gerados
└── libs/                             ← Dependências manuais
```

## 🎯 Executar Aplicação

```cmd
cd build\Release
.\SilverClinic.exe
```

## ⚡ Scripts Disponíveis

### PowerShell Scripts (Recomendado)
- `.\build_windows.ps1` - Build automatizado com opções avançadas
  ```powershell
  .\build_windows.ps1 -Configuration Release  # Build Release
  .\build_windows.ps1 -Configuration Debug    # Build Debug  
  .\build_windows.ps1 -Clean                  # Limpar e rebuild
  .\build_windows.ps1 -Help                   # Mostrar ajuda
  ```

- `.\run_app.ps1` - Executar aplicação principal
  ```powershell
  .\run_app.ps1                        # Executar Release
  .\run_app.ps1 -Configuration Debug   # Executar Debug
  ```

- `.\run_tests_windows.ps1` - Executar testes
  ```powershell
  .\run_tests_windows.ps1                     # Todos os testes
  .\run_tests_windows.ps1 -TestFilter beck*   # Só testes Beck
  .\run_tests_windows.ps1 -Verbose           # Output detalhado
  ```

### Batch Scripts (Alternativa)
- `build_windows.bat` - Build básico (Prompt de Comando)

## 🐛 Solução de Problemas

### Erro: "SQLite3 not found"
```cmd
# Instalar via vcpkg
vcpkg install sqlite3:x64-windows

# OU colocar manualmente em libs/
```

### Erro: "libharu not found"
```cmd
# Instalar via vcpkg
vcpkg install libharu:x64-windows

# OU baixar de: https://github.com/libharu/libharu/releases
```

### Erro: "Visual Studio not found"
- Instalar Visual Studio 2019/2022 Community
- Certificar que C++ CMake tools estão instalados

### Erro: "CMake not found"
- Adicionar CMake ao PATH
- Reiniciar terminal após instalação

## 📞 Suporte

Para problemas específicos do Windows, verificar:
1. Versão do Windows (10+ necessário)
2. Visual Studio corretamente instalado
3. Variáveis de ambiente (PATH, VCPKG_ROOT)
4. Permissões de administrador se necessário
