// Central mapping of validation / error codes to default messages (Portuguese-ready UI can translate later)
#ifndef SILVERCLINIC_VALIDATION_MESSAGES_H
#define SILVERCLINIC_VALIDATION_MESSAGES_H

#include <string>
#include <unordered_map>

namespace SilverClinic { namespace ValidationMessages {

inline const std::unordered_map<std::string, std::string>& map() {
    static const std::unordered_map<std::string, std::string> M = {
        {"validation.invalid_client_id", "Cliente inválido"},
        {"validation.invalid_assessor_id", "Avaliador inválido"},
        {"validation.invalid_notes_length", "Notas excedem tamanho máximo"},
        {"validation.assessor.invalid_fields", "Dados do avaliador inválidos"},
        {"db.prepare_failed", "Falha ao preparar comando SQL"},
        {"db.step_failed", "Falha ao executar comando SQL"},
        {"csv.missing_header", "Arquivo CSV com cabeçalho ausente"},
        {"csv.row_error", "Erro em linha do CSV"},
        {"csv.commit_failed", "Falha ao confirmar transação CSV"},
        {"id.allocation_failed", "Falha ao alocar novo ID"}
    };
    return M;
}

inline std::string messageFor(const std::string& code) {
    auto &m = map();
    auto it = m.find(code);
    return it == m.end() ? code : it->second;
}

} } // namespaces

#endif
