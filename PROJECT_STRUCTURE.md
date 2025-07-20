# SilverClinic - Estrutura do Projeto

## Visão Geral da Organização

O projeto SilverClinic está organizado de forma modular para facilitar a manutenção, expansão e compreensão do código.

## Estrutura de Diretórios

```
slt_cpp_programn/
├── include/                    # Headers (.h)
│   ├── core/                  # Classes principais do sistema
│   │   ├── DateTime.h         # Gerenciamento de data/hora
│   │   ├── Address.h          # Endereços
│   │   ├── Assessor.h         # Profissionais de saúde
│   │   ├── Client.h           # Clientes/Pacientes
│   │   ├── CaseProfile.h      # Perfis de caso
│   │   └── Utils.h            # Utilitários gerais
│   └── forms/                 # Classes dos formulários
│       ├── FamilyPhysician.h  # Médico da família
│       ├── EmergencyContact.h # Contato de emergência
│       ├── LegalRepresentative.h # Representante legal
│       └── InsuranceInfo.h    # Informações de seguro
├── src/                       # Implementações (.cpp)
│   ├── core/                  # Implementações das classes principais
│   │   ├── DateTime.cpp
│   │   ├── Address.cpp
│   │   ├── Assessor.cpp
│   │   ├── Client.cpp
│   │   ├── CaseProfile.cpp
│   │   └── Utils.cpp
│   ├── forms/                 # Implementações dos formulários
│   │   ├── FamilyPhysician.cpp
│   │   ├── EmergencyContact.cpp
│   │   ├── LegalRepresentative.cpp
│   │   └── InsuranceInfo.cpp
│   └── main.cpp              # Ponto de entrada da aplicação
├── forms_html/               # Templates HTML dos formulários
│   └── case-info.html        # Formulário de informações do caso
├── build/                    # Arquivos de build (CMake)
├── docs/                     # Documentação adicional
└── tests/                    # Testes unitários (futuro)
```

## Categorização das Classes

### Core Classes (Entidades Principais)
- **DateTime**: Gerenciamento de timestamps e cálculos de data
- **Address**: Endereços com validação canadense
- **Assessor**: Profissionais de saúde mental/assistentes sociais
- **Client**: Clientes do sistema (pacientes)
- **CaseProfile**: Perfis de caso conectando clientes e assessores
- **Utils**: Funções utilitárias e validação de dados

### Form Classes (Formulários de Dados)
- **FamilyPhysician**: Informações do médico da família
- **EmergencyContact**: Contatos de emergência
- **LegalRepresentative**: Representantes legais
- **InsuranceInfo**: Informações de seguro de saúde

## Benefícios da Organização

### 1. **Separação de Responsabilidades**
- Classes core: Lógica de negócio fundamental
- Classes forms: Coleta e validação de dados específicos

### 2. **Facilidade de Manutenção**
- Cada categoria tem seu próprio namespace
- Mudanças em formulários não afetam core
- Fácil localização de código específico

### 3. **Escalabilidade**
- Novos formulários podem ser adicionados facilmente
- Novas funcionalidades core não interferem em forms
- Estrutura preparada para crescimento

### 4. **Reutilização de Código**
- Utils compartilhado entre todas as classes
- Padrões consistentes entre formulários
- Validações padronizadas

## Padrões de Nomenclatura

### Arquivos
- Headers: `ClassName.h`
- Implementações: `ClassName.cpp`
- Sempre em PascalCase

### Namespaces
- Core: `SilverClinic::`
- Forms: `SilverClinic::Forms::`
- Utils: `utils::`

### Prefixos de ID
- Assessor: 100000+
- Address: 200000+
- Client: 300000+
- CaseProfile: 400000+
- FamilyPhysician: 500000+
- EmergencyContact: 600000+
- LegalRepresentative: 700000+
- InsuranceInfo: 800000+

## Próximos Passos

1. **Reorganizar arquivos existentes** para a nova estrutura
2. **Implementar classes de formulários** baseadas no HTML
3. **Atualizar CMakeLists.txt** para incluir novos diretórios
4. **Criar testes unitários** para cada categoria
5. **Implementar interface de usuário** integrada

## CMake Integration

O CMakeLists.txt será atualizado para:
- Detectar automaticamente arquivos nas subpastas
- Incluir os novos diretórios de headers
- Manter compatibilidade com a estrutura atual
