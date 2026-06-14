# UFCLite — аркадный файтинг

## Суть
Проект файтинга на UE5.7.4 (C++). 3D вид сбоку, два бойца, удары руками/ногами, блок, кровь/увечья на моделях. Геймпад. Никакой прогрессии.

## Состояние
- **UE5.7.4 C++ проект UFCLite создан на ПК**
- Есть PLAN.md — полная спецификация
- Есть Source/UFCLite/FighterCharacter.h — начальный C++ класс
- Проект залит на GitHub (main)
- .gitignore добавлен
- **В работе:** добавление C++ классов

## Критично
При старте новой сессии:
1. Прочитать этот файл + PLAN.md
2. Прочитать Source/UFCLite/* (все текущие C++ файлы)
3. Продолжить добавлять классы: FighterCharacter.cpp, HealthComponent, DamageSystem, FightGameMode, FightHUD, BloodSystem, FighterSelectWidget, MainPlayerController

## Порядок действий (начать новую сессию)
1. Прочитать PLAN.md
2. Прочитать Source/UFCLite/* (все текущие C++ файлы)
3. Продолжить добавлять классы: FighterCharacter.cpp, HealthComponent, DamageSystem, FightGameMode, FightHUD, BloodSystem, FighterSelectWidget, MainPlayerController
4. После каждого блока — запросить пользователя закоммитить и запушить с ПК

## Контекст пользователя
- GitHub: delyev3852
- ПК: Windows, UE5.7.4, RTX 5060, 16GB RAM
- Пользователь не умеет программировать — давать чёткие инструкции для UE5 (Create C++ class → назвать имя → скопировать код)

## Правила работы
- После добавления каждого C++ класса → инструкция пользователю: «Открой Visual Studio → создай класс → замени содержимое на код из репозитория → скомпилируй»
- Держать код компилируемым
- Не использовать платные ассеты
- Персонажи — из Mixamo (бесплатный UE5 плагин или FBX)
