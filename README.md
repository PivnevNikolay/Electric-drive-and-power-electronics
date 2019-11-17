
## Устройство предназначено для проверки драйверов 
Power DRIVER ДТИ- 1/12 “Элсиэл” СМПК.758724.024-01
--------------------------------------------------
Проект создан для проверки платы драйвера ДТИ - 1/12.
Устройство представляет собой текстолитовую пластину с размещёнными на ней источниками питания ,
триггерными кнопками, двумя микроконтроллерами (XMC1100T038X0064 и STM8S103F3P6) и транзисторами.
В замен силового транзистора IGBT SKM150GB используем два  IGBT транзистора IRGS460, AUIRGP50B60PD1.

![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/001.jpg "bot")
***
![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/002.jpg "top")

## Технические характеристики

+ Габаритные размеры 100/100 мм
+ Напряжение питания: 24V (min 0.7A).
+ Произведено на https://www.seeedstudio.com/ 
+ Микроконтроллеры: XMC1100T038X0064, STM8S103F3P6
+ STM8S103F3P6 --> 16 MHz; 8 Kbyte Flash;
+ XMC1100T038X0064 -->  ARM Cortex M0; 32 MHz; 64 Kbyte Flash;

## Список компонентов
???
## Подключение и управление
![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/004.jpg "bot")
Из органов управления на устройстве размещены две триггерных кнопки:
1. Start-Stop Button (Включение и выключение соответственно); 
2. Sin-PWM Button (Выбор режима работы);
+ Нажатие кнопки подсвечивается светодиодом!Визуализация: построена на светодиодах согласно программ микроконтроллеров.
## Структура проекта
1. Папка Code содержит файлы с расширеним .hex для прошивки МК(STM8, XMC1100).
2. Папка Docs содержит фалы с расширением .pdf (схемы, текстовые описания).
3. Папка Hardware содержит Gerber который представляет собой способ описания проекта печатной платы для изготовления на производстве.
4. BOM.xlsx спецификация.
## Контакты
+ E-mail: pivnevtosno@yandex.ru
