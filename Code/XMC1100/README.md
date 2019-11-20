## Прошивка XMC1100 с помощью XMCFlasher (загрузка в XMC1100 файла с расширением .hex).
1. Необходимо приобрести любую отладочную плату от Infineon серии XMC1000.
В моём случае это отладочная плата Boot Kit XMC1300
(https://www.infineon.com/cms/en/product/evaluation-boards/kit_xmc13_boot_001/)
2. Отломить программатор от отладочнай платы(разламив оладку попалам).
3. Подсоеденить выводы программатора к соответствующим выводам устройства для проверки драйвера ДТИ 1/12. 
![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/011_XMC1100.jpg "bot")
4. Запустить XMC Flasher.
![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/008_XMC1100.jpg "bot")
5. Зайти в меню BMI(BMI Get Set), нажать Get BMI выйдет ASC Bootstrap Loader mode (ASC_BSL).
![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/009_XMC1100.jpg "bot")
6. Напротив Set BMI выбрать User Mode (Debug)SWD0 и нажать Set BMI. Далее нажать CLOSE.
7. Заново зайти в это меню (BMI Get Set). Нажать Get BMI. Напротив должно высветится User Mode(Debug)SWD0(SWDIO=P0.14, SWDCLK=P0.15)
![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/010_XMC1100.jpg "bot")
8. Далее нажать CLOSE.
9. Нажать Connect выбрать XMC1100-0064 , OK. 
10. Select File выбрать файл с расширением .hex (XMC1100_DTI_1_12.hex). Нажать Program. Подождать и далее нажать Disconnect. 
![alt-текст](https://github.com/PivnevNikolay/PTK_2M2_V_DTI_1_12/blob/master/foto/012_XMC1100.jpg "bot")
