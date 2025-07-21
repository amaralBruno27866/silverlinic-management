# 🧪 Guia de Testes - SilverClinic Management System

Este guia explica como executar testes no VS Code de forma integrada.

## 🚀 Configuração Completa

O projeto está configurado com:
- ✅ **CMake Tools**: Para build automatizado
- ✅ **CMake Test Explorer**: Para visualizar e executar testes
- ✅ **Tasks.json**: Tarefas predefinidas
- ✅ **Launch.json**: Configurações de debug
- ✅ **Settings.json**: Configurações do projeto

## 📋 Como Executar Testes

### Método 1: Usando Command Palette (Recomendado)

1. **Abra o Command Palette**: `Ctrl+Shift+P`
2. **Digite**: `Tasks: Run Task`
3. **Selecione uma das opções**:
   - `Run All Tests` - Executa todos os testes
   - `Test: DateTime` - Testa apenas DateTime
   - `Test: Address` - Testa apenas Address  
   - `Test: Assessor` - Testa apenas Assessor
   - `Test: Utils` - Testa apenas Utils
   - `Test: Core Classes` - Testa classes core
   - `Test: FamilyPhysician` - Testa FamilyPhysician
   - `Test: Database Integration` - Testa integração DB

### Método 2: Usando Atalhos de Teclado

1. **Build**: `Ctrl+Shift+P` → `CMake: Build`
2. **Run Tests**: `Ctrl+Shift+P` → `Tasks: Run Test Task`

### Método 3: Usando Terminal Integrado

```bash
# No terminal integrado do VS Code (Ctrl+`)
chmod +x test_simple.sh
./test_simple.sh
```

### Método 4: CMake Test Explorer (Sidebar)

1. **Abra a extensão CMake Test Explorer** na sidebar
2. **Configure o projeto** (primeira vez)
3. **Clique nos testes** para executar individualmente

## 🐛 Debug de Testes

### Para debugar um teste específico:

1. **Vá para a aba Run and Debug** (`Ctrl+Shift+D`)
2. **Selecione a configuração desejada**:
   - `Debug DateTime Tests`
   - `Debug Address Tests`
   - `Debug Assessor Tests`
   - `Debug Utils Tests`
   - etc.
3. **Pressione F5** ou clique no botão play

### Colocando Breakpoints:

1. **Abra o arquivo de teste** (ex: `tests/unit/test_datetime.cpp`)
2. **Clique na margem esquerda** da linha desejada
3. **Execute o debug** da configuração correspondente

## 📊 Estrutura dos Testes

```
tests/
├── unit/                    # Testes unitários
│   ├── test_datetime.cpp   # Testes da classe DateTime
│   ├── test_address.cpp    # Testes da classe Address
│   ├── test_assessor.cpp   # Testes da classe Assessor
│   ├── test_utils.cpp      # Testes das funções Utils
│   ├── test_core_classes.cpp
│   └── test_family_physician.cpp
└── integration/             # Testes de integração
    └── test_database.cpp   # Testes do banco de dados
```

## 🔧 Troubleshooting

### Problema: "Executável não encontrado"
**Solução**: Execute `CMake: Build` primeiro
1. `Ctrl+Shift+P` → `CMake: Configure`
2. `Ctrl+Shift+P` → `CMake: Build`

### Problema: "CMake não configurado"
**Solução**: Configure o CMake
1. `Ctrl+Shift+P` → `CMake: Select a Kit`
2. Escolha o compilador (geralmente GCC)
3. `Ctrl+Shift+P` → `CMake: Configure`

### Problema: "SQLite3 não encontrado"
**Solução**: Instale SQLite3
```bash
sudo apt update
sudo apt install libsqlite3-dev
```

### Problema: Script sem permissão
**Solução**: Torne executável
```bash
chmod +x run_tests.sh
chmod +x test_simple.sh
```

## 📈 Saída dos Testes

Os testes produzem saída colorida com emojis:
- ✅ **PASS**: Teste passou
- ❌ **FAIL**: Teste falhou  
- 🧪 **Running**: Executando teste
- 📊 **Summary**: Resumo final

### Exemplo de Saída:
```
🚀 DateTime Unit Tests
=====================

🧪 Running test_default_constructor...
✅ PASS: Default constructor should set current time
✅ test_default_constructor completed successfully

📊 Test Summary
===============
Total tests: 7
Passed: 7 ✅
Failed: 0 ❌
Success rate: 100%

🎉 All tests passed!
```

## 🎯 Casos de Teste Incluídos

### DateTime (7 testes)
- Construtor padrão
- Construtor parametrizado
- Método now()
- Cálculo de idade
- Casos extremos
- Formatação de string
- Consistência

### Address (8 testes)
- Construtores
- Geração de ID
- Setters/Getters
- Formatação de código postal
- Métodos de display
- Casos extremos
- Timestamps

### Assessor (11 testes)
- Construtores
- Geração de ID
- Nome completo
- Integração com Address
- Cenários de email
- Cenários de telefone
- Credenciais
- Display

### Utils (7 testes)
- Formatação de telefone
- Validação de email
- Validação de código postal
- Trimming de strings
- Geração de timestamps
- Conversão de case
- Validações especializadas

## 🚀 Próximos Passos

1. **Execute os testes** usando os métodos acima
2. **Analise a saída** para identificar problemas
3. **Use o debug** para investigar falhas
4. **Adicione novos testes** conforme necessário

---

**💡 Dica**: Mantenha os testes executando regularmente durante o desenvolvimento para detectar problemas cedo!
