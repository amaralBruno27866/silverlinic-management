# Silver Clinic - Arquitetura do Sistema

## 🏗️ Estrutura de Classes

```
                           SILVER CLINIC SYSTEM
                           ═══════════════════════
                                    │
                     ┌──────────────┼──────────────┐
                     │              │              │
               ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
               │  ASSESSOR   │ │   ADDRESS   │ │   CLIENT    │
               │  (100001+)  │ │  (200001+)  │ │  (300001+)  │
               └─────────────┘ └─────────────┘ └─────────────┘
                     │              │              │
                     └──────────────┼──────────────┘
                                    │
                              ┌─────────────┐
                              │    UTILS    │
                              │ (Validação) │
                              └─────────────┘
```

## 📊 Relacionamentos do Banco de Dados

```sql
┌─────────────────────────────────┐
│           ASSESSOR              │
├─────────────────────────────────┤
│ id: 100001+ (PK)                │
│ firstname: VARCHAR              │
│ lastname: VARCHAR               │
│ email: VARCHAR                  │
│ created_at: DATETIME            │
│ updated_at: DATETIME            │
└─────────────────────────────────┘
                │
                │ 1:1
                ▼
┌─────────────────────────────────┐
│           ADDRESS               │
├─────────────────────────────────┤
│ id: 200001+ (PK)                │
│ user_key: INT (FK)              │ ◄─── Referencia assessor.id
│ street: VARCHAR                 │      ou client.id
│ city: VARCHAR                   │
│ province: VARCHAR               │
│ postal_code: VARCHAR            │
│ created_at: DATETIME            │
│ updated_at: DATETIME            │
└─────────────────────────────────┘
                ▲
                │ 1:1
                │
┌─────────────────────────────────┐
│            CLIENT               │
├─────────────────────────────────┤
│ id: 300001+ (PK)                │
│ firstname: VARCHAR              │
│ lastname: VARCHAR               │
│ email: VARCHAR                  │
│ health_card: VARCHAR            │
│ sin: VARCHAR                    │
│ date_of_birth: DATE             │
│ created_at: DATETIME            │
│ updated_at: DATETIME            │
└─────────────────────────────────┘
```

## 🔑 Sistema de IDs

### Prefixos e Intervalos:
| Entidade  | Prefixo | Range     | Próximo ID | Status   |
|-----------|---------|-----------|------------|----------|
| Assessor  | 100000  | 100001+   | 100002     | ✅ Ativo |
| Address   | 200000  | 200001+   | 200003     | ✅ Ativo |
| Client    | 300000  | 300001+   | 300002     | ✅ Ativo |

### Algoritmo de Geração:
```cpp
// Para cada classe
static int current_id = 0;
static const int ID_PREFIX = XXX000; // 100000, 200000, 300000

int getNextId() {
    return ID_PREFIX + (++current_id);
}
```

## 📁 Estrutura de Arquivos

```
slt_cpp_programn/
├── include/
│   ├── Address.h      ✅ Headers de classes
│   ├── Assessor.h     ✅
│   ├── Client.h       ✅ [NOVO]
│   ├── DateTime.h     ✅
│   └── Utils.h        ✅
├── src/
│   ├── Address.cpp    ✅ Implementações
│   ├── Assessor.cpp   ✅
│   ├── Client.cpp     ✅ [NOVO] 
│   ├── DateTime.cpp   ✅
│   ├── main.cpp       ✅ [REFORMULADO]
│   └── Utils.cpp      ✅
├── build/             ✅ Arquivos de build
├── CMakeLists.txt     ✅ Configuração CMake
└── clinic.db          ✅ Banco SQLite
```

## 🛠️ Funcionalidades por Módulo

### 📋 **Utils.h/Utils.cpp**
```cpp
• Validação de código postal canadense
• Formatação de telefone (416) 555-0123
• Validação de SIN com algoritmo Luhn
• Geração de IDs aleatórios
• Sistema de logging
• Formatação de datas/horários
• Limpeza de strings
• Validação de emails
```

### 👨‍⚕️ **Assessor.h/Assessor.cpp**
```cpp
• ID: 100001+ (Médicos, enfermeiros, staff)
• Nome completo (firstname + lastname)
• Email profissional
• Validação de dados
• Timestamps automáticos
```

### 🏠 **Address.h/Address.cpp**
```cpp
• ID: 200001+ (Endereços)
• user_key: FK para Assessor ou Client
• Endereço completo canadense
• Validação de código postal
• Relacionamento 1:1 com usuários
```

### 👤 **Client.h/Client.cpp**
```cpp
• ID: 300001+ (Pacientes)
• Dados pessoais completos
• health_card: Cartão de saúde
• sin: Social Insurance Number
• date_of_birth: Data de nascimento
• Cálculo automático de idade
• Mascaramento de dados sensíveis
• Validações específicas do sistema de saúde
```

### 📅 **DateTime.h/DateTime.cpp**
```cpp
• Manipulação de datas/horários
• Formatação personalizada
• Cálculos de idade e intervalos
• Timestamps para auditoria
```

## 🔐 Segurança e Validações

### **Dados Mascarados:**
- SIN: `123-456-789` → `123******`
- Health Card: `1234567890` → `1234******`

### **Validações Canadenses:**
- ✅ Código Postal: `K1A 0A6`, `M5V 3A8`
- ✅ Telefone: `(416) 555-0123`
- ✅ SIN: Algoritmo Luhn + formato
- ✅ Email: Formato padrão

### **Integridade Referencial:**
- `address.user_key` → `assessor.id` OU `client.id`
- Relacionamento 1:1 garantido
- Validação de chaves estrangeiras

## 🚀 Estado Atual

### ✅ **Implementado:**
- Sistema de IDs com prefixos
- Classes completas (Assessor, Client, Address)
- Banco de dados SQLite3 integrado
- Validações canadenses
- Segurança de dados
- Relacionamentos corretos

### 🎯 **Próximos Passos:**
1. Interface de usuário interativa
2. Operações CRUD completas
3. Sistema de agendamentos
4. Relatórios e estatísticas
5. Backup e recuperação

---
**✨ Silver Clinic está pronto para desenvolvimento avançado!** 🎉
