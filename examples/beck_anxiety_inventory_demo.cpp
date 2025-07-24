#include "forms/BeckAnxietyInventory.h"
#include <iostream>
#include <cassert>

using namespace SilverClinic::Forms;
using namespace std;

/**
 * @brief Exemplo simples de uso do Beck Anxiety Inventory
 * 
 * Este exemplo demonstra como:
 * - Criar uma instância do BAI
 * - Definir respostas para as questões
 * - Calcular pontuação e nível de severidade
 * - Interpretar os resultados clinicamente
 */
int main() {
    cout << "=== Beck Anxiety Inventory (BAI) - Exemplo de Uso ===" << endl;
    
    // Resetar contador para demonstração
    BeckAnxietyInventory::resetIdCounter();
    
    // Criar instância do BAI para um caso específico
    BeckAnxietyInventory bai(12345); // Case Profile ID = 12345
    
    cout << "\n📋 Informações Básicas:" << endl;
    cout << "BAI ID: " << bai.getBAIId() << endl;
    cout << "Tipo: " << bai.getType() << endl;
    cout << "Case Profile ID: " << bai.getCaseProfileId() << endl;
    
    // Simular respostas de um paciente com ansiedade moderada
    cout << "\n📝 Simulando respostas do paciente..." << endl;
    bai.setQuestion1(2);  // Formigamento/dormência
    bai.setQuestion2(1);  // Sensação de calor
    bai.setQuestion3(2);  // Tremor nas pernas
    bai.setQuestion4(1);  // Incapaz de relaxar
    bai.setQuestion5(2);  // Medo que aconteça o pior
    bai.setQuestion6(1);  // Tontura ou vertigem
    bai.setQuestion7(2);  // Palpitação ou aceleração do coração
    bai.setQuestion8(1);  // Sem equilíbrio
    bai.setQuestion9(2);  // Aterrorizado
    bai.setQuestion10(1); // Nervoso
    bai.setQuestion11(2); // Sensação de sufocação
    bai.setQuestion12(1); // Tremor nas mãos
    bai.setQuestion13(2); // Trêmulo
    bai.setQuestion14(1); // Medo de perder o controle
    bai.setQuestion15(2); // Dificuldade para respirar
    bai.setQuestion16(1); // Medo de morrer
    bai.setQuestion17(0); // Assustado
    bai.setQuestion18(1); // Indigestão ou desconforto abdominal
    bai.setQuestion19(1); // Desmaiar
    bai.setQuestion20(1); // Rosto afogueado
    bai.setQuestion21(0); // Suor (não devido ao calor)
    
    // Exibir resultados
    cout << "\n📊 Resultados da Avaliação:" << endl;
    cout << "Pontuação Total: " << bai.getTotalScore() << "/63" << endl;
    cout << "Porcentagem de Ansiedade: " << fixed << setprecision(1) << bai.getAnxietyPercentage() << "%" << endl;
    cout << "Nível de Severidade: " << bai.getSeverityLevel() << endl;
    
    // Análise por categorias
    cout << "\n🔍 Análise por Categorias:" << endl;
    cout << "Sintomas Cognitivos: " << bai.getCognitiveScore() << "/21" << endl;
    cout << "Sintomas Afetivos: " << bai.getAffectiveScore() << "/21" << endl;
    cout << "Sintomas Somáticos: " << bai.getSomaticScore() << "/21" << endl;
    
    // Interpretação clínica
    cout << "\n🏥 Interpretação Clínica:" << endl;
    if (bai.hasMinimalAnxiety()) {
        cout << "✅ Ansiedade Mínima - Dentro dos parâmetros normais" << endl;
    } else if (bai.hasMildAnxiety()) {
        cout << "⚠️ Ansiedade Leve - Monitoramento recomendado" << endl;
    } else if (bai.hasModerateAnxiety()) {
        cout << "🔶 Ansiedade Moderada - Intervenção terapêutica indicada" << endl;
    } else if (bai.hasSevereAnxiety()) {
        cout << "🔴 Ansiedade Severa - Tratamento intensivo necessário" << endl;
    }
    
    if (BeckAnxietyInventory::isHighRiskScore(bai.getTotalScore())) {
        cout << "⚠️ ALERTA: Pontuação indica alto risco - Avaliação clínica urgente!" << endl;
    }
    
    // Exibir timestamp
    cout << "\n⏰ Timestamps:" << endl;
    cout << "Criado em: " << bai.getBAICreatedAt().toString() << endl;
    cout << "Última modificação: " << bai.getBAIUpdatedAt().toString() << endl;
    
    cout << "\n✅ Exemplo concluído com sucesso!" << endl;
    return 0;
}
