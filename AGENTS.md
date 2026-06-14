# UFCLite — аркадный файтинг (UE5.7.4 C++)

## Состояние (15.06.2026, конец сессии №2)

### Проект
- UE5.7.4 C++ проект в `C:\UFCLite`, GitHub `delyev3852/UFCLite`
- Компилируется, игра запускается через Play

### Готовые C++ классы
- **FighterCharacter.h/.cpp** — персонаж: передвижение (world‑space: A/D=ось X, W/S=ось Y, W=вперёд), все удары, блок (скорость ×0.5), смерть, цилиндр-заглушка
- **HealthComponent.h/.cpp** — здоровье + выносливость, реген, репликация, `OnDeath`/`OnHealthChanged`
- **FightGameMode.h/.cpp** — спавн арены (500×50×0.5 Cube), спавн бойцов по (±150, 0, 100), камера (0, 500, 200), HUD класс
- **FightHUD.h/.cpp** — полоски здоровья (красная) и выносливости (синяя) слева/справа
- **MainPlayerController.h/.cpp** — заглушка

### Ввод
- **Enhanced Input** — `UInputAction`/`UInputMappingContext` создаются в `BeginPlay` через `NewObject`
- **UFC-style chorded**: кнопки лица = ведущие удары, R2+кнопка = удары ногами, R1 = блок
- Клавиатура: X/Y/B/A = удары, R2/KickModifier пока нет на клаве, Shift = блок
- Legacy BindAxisKey для левого стика геймпада (запасной путь)
- Legacy BindAction для геймпада (Jab/Cross/Hook/Uppercut/LowKick/MidKick/HighKick/Block)
- `DefaultInput.ini` — `EnhancedPlayerInput` + `EnhancedInputComponent`, ActionMappings для геймпада

### Камера
- Одна общая камера `ACameraActor` на сцену, оба игрока смотрят через `SetViewTarget`
- Без сплит-скрина, без CreatePlayer

### Проблемы
- **Геймпад (DualSense) не работает.** UE5 не видит контроллер:
  - DualSense подключён по USB
  - Steam выключен
  - Работает в других играх и Steam
  - Плагин `Windows.GamingInput` отсутствует в UE5.7
  - `FWindowsPlatformApplicationMisc::IsGamepadAttached()` — нет такого метода
  - `FSlateApplication::Get().IsGamepadAttached()` — требует линковки модуля Slate
  - `joyGetPosEx` — синтаксический конфликт с Windows SDK
  - Console variables `input.bEnableDirectInput=1` и т.д. не помогли
  - **Вероятная причина:** Windows.Gaming.Input не инициализируется для DualSense на этой системе
  - **Варианты решения:**
    1. Установить Xbox Game Bar / Gaming Services из Microsoft Store (дают Windows.Gaming.Input runtime)
    2. Использовать C++/WinRT с `Windows::Gaming::Input::Gamepad` напрямую
    3. DS4Windows (пользователь хотел избежать)
    4. Запускать UE5 через Steam как non-Steam game (Steam Input эмулирует XInput)

### Нужно сделать
1. Починить геймпад
2. Скелетные меши + анимации из Mixamo
3. Hit detection / DamageSystem
4. BloodSystem (декали крови)
5. Экран выбора бойца
6. Второй игрок (клавиатура или 2-й геймпад)

### При старте сессии
1. Прочитать AGENTS.md + PLAN.md
2. Прочитать все Source/UFCLite/* файлы
3. git pull + Build + Play
4. Начать с пункта выше

### Контекст
- GitHub: delyev3852
- ПК: Windows, UE5.7.4, RTX 5060, 16GB RAM
- Пользователь не программист — давать пошаговые инструкции
