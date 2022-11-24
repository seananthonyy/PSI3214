/*
 * Author: Rodrigo Anjos de Souza
 * Data: 17/10/2019
 * 
 * Descrição: O codigo abaixo configura o Arduino UNO para que 
 * realize a captura e o envio de dados com uma taxa de amostragem
 * de aproximadamente 75 kHz e com uma resolução do conversor AD
 * de 8-bits.
 * 
 * Na rotina de interrupção do ADC, enviamos N amostras 
 * 
 * Os comentários à direita dos comandos utilizados fazem referência
 * às páginas correnspondentes no datasheet do microcontrolador, 
 * indicado abaixo:
 * 
 * http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
 * 
 */

/* Inclusão das bibliotecas do microcontrolador */
#include <avr/io.h>


/* Definição das mascaras utilizadas para configurar 
 * a porta de leitura analógica 
*/
#define MUXMASK 0x07                                    // (p. 262, 263)
#define ADC1 0b0001
#define ADC0 0b0000

/* Variáveis auxiliares para determinar quando será enviado '\n',  
 * definição da quantidade de pontos enviados por bloco.
 */
volatile uint8_t i = 0;
#define N 200

/* Inicialização do microcontrolador 
 * 
 * Neste etapa são configurados os principais parâmetros 
 * do microcontrolador
 * 
 */
void setup() 
{
  /* Configuração do Baudrate da porta Serial */
  Serial.begin(2000000);
  
  /* Configuração do Conversor AD
   * 
   * Nesta etapa serão configurados os parâmetros relacionados
   * ao processo de amostragem
   * 
   */
  
  /* Limpar os registradores principais do ADC */       // (p. 263)
  ADCSRA = 0x00;
  ADCSRB = 0x00;
  ADMUX  = 0x00;
  
  /* Definição da porta analógica para captura do sinal: A0 */                      
  ADMUX  |= (ADC0 & MUXMASK);                           // (p. 262)
  
  /* Definição da tensão de fundo de escala: 5V */
  ADMUX  |= (1 << REFS0);                               // (p. 262)
  
  /* Definição de como o valor do ADC é guardado */
  ADMUX  |= (1 << ADLAR);                               // (p. 262, 265)
  
  /* Definição do 'prescale' do clock do ADC: 16 */
  /* Fa = F_CPU/(13.5*prescale) =~ 75 kHz */
  ADCSRA |= (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0); // (p. 264, 255)
  
  /* Definição do modo de operação do ADC: Free Running */
  ADCSRB |= (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0); // (p. 265, 266, 253)
  
  /* Habilitação do Auto-Trigger do ADC */
  ADCSRA |= (1 << ADATE);                               // (p. 264)
  
  /* Habilitação da rotina de interrupção a cada amostragem */
  ADCSRA |= (1 << ADIE);                                // (p. 264)
  
  /* Habilitação do ADC */
  ADCSRA |= (1 << ADEN);                                // (p. 263)
  
  /* Inicialização do ADC */
  ADCSRA |= (1 << ADSC);                                // (p. 263)
}

/* Rotina de Interrupção do ADC */
/*  
 * Essa função é executada toda vez que o conversor termina de realizar
 * uma amostragem. Como o ADC foi configurado para que realize a amostragem
 * continuamente, a função abaixo é executada initerruptamente.
 */
ISR(ADC_vect)
{
  /* Incrementa contador de amostras */
  i++;

  /* Envia, em bits, o valor guardado no registrador do ADC de 8-bits */
  Serial.write(ADCH); 

  if(i >= N){   /* Envia o caracter de terminação utilizado pelo LabVIEW e reseta o contador*/
    Serial.write('\n');
    i = 0;
  }
}

void loop() 
{
  // A função Loop não utilizada, mas sua definição é necesária na IDE do Arduino
}

