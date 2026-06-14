# UFCLite — аркадный файтинг (UE5.7.4 C++)

## Состояние (16.06.2026, конец сессии №3)

### Готово
- ✅ Phase 0 (фундамент): GameMode, персонаж, HUD, Enhanced Input, камера
- ✅ Enhanced Input — UFC-style chorded (face buttons = lead strikes, R2+face = kicks, R1 = block)
- ✅ FighterAnimInstance C++ класс (Speed, Direction, bIsBlocking, bIsDead)
- ✅ Цилиндр заменён на скелетный манекен UE5 (`/Engine/EngineMeshes/SkeletalMannequin`)
- ✅ AnimInstance привязан через `GetMesh()->AnimClass = UFighterAnimInstance::StaticClass()`

### Баги
- ❌ **Клавиша A конфликтует**: привязана к MoveRightAction (движение влево) И FaceBottomAction (апперкот)
  - При движении влево тратится стамина (срабатывает апперкот)
  - Фикс: перераспределить клавиши ударов (не использовать A/B для атак)

- ❌ **Пол арены не отображается** — возможно AStaticMeshActor не спавнится корректно
  - Причина: `SetMobility(EComponentMobility::Stationary)` после спавна может ломать рендер
  - Фикс: поменять на `EComponentMobility::Movable` или настроить в конструкторе

- ❌ **Геймпад (DualSense) не определяется UE5**
  - Были попытки: WGI plugin, DirectInput CVar, FSlateApplication, WinMM, joyGetPosEx — не помогли
  - Вероятная причина: C++/WinRT `Windows::Gaming::Input` не инициализирован для DualSense
  - Решение: отложено, используем клавиатуру

### Нужно сделать (следующий сеанс)
1. Переделать раскладку клавиатуры — убрать конфликт A/B/X/Y с ударами
   - Новая схема: WASD = движение, Z/X/C/V = удары, Q/E/R/T = ноги, Shift = блок
2. Починить спавн пола арены (Mobility → Movable)
3. Настроить анимации: скачать Mixamo → импортировать → привязать к монтажам
4. Сделать хитбоксы (SphereTrace в момент удара)
5. DamageSystem

### Файлы проекта
- `FighterCharacter.h/.cpp` — персонаж, ввод, удары, блок
- `FighterAnimInstance.h/.cpp` — анимационный C++ класс
- `HealthComponent.h/.cpp` — здоровье + выносливость
- `FightGameMode.h/.cpp` — спавн, камера, игроки
- `FightHUD.h/.cpp` — полоски здоровья/выносливости
- `MainPlayerController.h/.cpp` — заглушка
- `DefaultInput.ini` — Input config (Enhanced + legacy gamepad bindings)
- `DefaultEngine.ini` — GameMode, console vars
- `UFCLite.uproject` — EnhancedInput plugin

### Примечания
- При старте: `git pull` → проверить изменения
- После изменения Build.cs: regenerate VS project files (.uproject → Generate)
