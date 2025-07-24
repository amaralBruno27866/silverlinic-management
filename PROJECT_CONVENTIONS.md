# 🎯 CONVENÇÕES DE PROJETO - SILVER CLINIC

## ⚠️ LEIA ANTES DE CRIAR QUALQUER ARQUIVO

### 🗂️ ESTRUTURA OBRIGATÓRIA:

```
📁 data/              ← TODA persistência
  ├── clinic.db       ← Banco principal (ÚNICO)
  ├── pdfs/           ← TODOS os PDFs
  └── test/           ← TODOS os bancos de teste

📁 tests/             ← TODOS os testes
  ├── unit/           ← Testes unitários
  └── integration/    ← Testes de integração

📁 src/               ← TODAS as implementações
  ├── core/           ← Classes principais
  ├── forms/          ← Formulários
  ├── managers/       ← Gerenciadores
  └── utils/          ← Utilitários

📁 include/           ← TODOS os headers
  ├── core/           ← Headers principais
  ├── forms/          ← Headers formulários
  ├── managers/       ← Headers gerenciadores
  └── utils/          ← Headers utilitários

📁 examples/          ← TODOS os exemplos
📁 documents/         ← TODA documentação
```

### 🚨 NUNCA FAÇA:

❌ Criar bancos `.db` fora de `data/`
❌ Criar PDFs fora de `data/pdfs/`
❌ Criar testes fora de `tests/`
❌ Criar `.cpp` na raiz (exceto `main.cpp`)
❌ Criar arquivos sem consultar esta estrutura

### ✅ SEMPRE FAÇA:

✅ Consulte `documents/ARCHITECTURE.md` primeiro
✅ Use a estrutura de pastas definida
✅ Mantenha organização por categoria
✅ Teste após reorganizar arquivos

### 📋 CHECKLIST RÁPIDO:

**Arquivo é:**
- 🧪 Teste? → `tests/unit/` ou `tests/integration/`
- 🗄️ Banco/PDF? → `data/` ou `data/pdfs/`
- 📄 Header? → `include/categoria/`
- ⚡ Implementação? → `src/categoria/`
- 📚 Documentação? → `documents/`
- 💡 Exemplo? → `examples/`

### 🔍 COMANDO VERIFICAÇÃO:

```bash
# Verificar estrutura atual
find . -name "*.cpp" -o -name "*.db" -o -name "*.pdf" | grep -v build | sort
```

**🎯 Meta: Zero arquivos fora da estrutura definida**
