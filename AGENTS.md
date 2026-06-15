# UFCLite — аркадный файтинг (UE5.7.4 C++)

## Состояние (16.06.2026, конец сессии)

### Готово
- ✅ Фундамент: GameMode, персонажи, HUD, камера, ввод
- ✅ Enhanced Input: UFC-style chorded (Z/X/C/V = punches, Tab+кнопка = kicks, Shift = block)
- ✅ P1: WASD + Z/X/C/V/Tab/Shift
- ✅ P2: Стрелки + I/O/P/K/L/RCtrl (Raw Input через GetAsyncKeyState + AddActorWorldOffset)
- ✅ Hit detection: SphereTrace, урон, отбрасывание, блок (×0.3), кровь (debug сфера+линии)
- ✅ KO: экран "PLAYER X WINS!", авто-рестарт через 5 сек
- ✅ Октагон: круглый пол-диск + 8 столбов + 3 уровня перекладин
- ✅ Камера: следит за обоими бойцами (между ними)

### FBX из Mixamo (в процессе)
- 13 FBX-файлов лежат в `C:\UFCLite\fbx\`
- Код авто-импорта в `FightGameMode` — забирает из fbx/ → Content/Characters/
- Первый импорт упал: материал Alpha_Body_MAT вызвал ассерт
- **Исправлено:** bImportMaterials = false, bReplaceExisting = true
- **Нужно сделать:**
  1. В Content Browser удалить битые файлы из `Characters/`
  2. Нажать Play → чистый реимпорт без материалов
  3. Проверить — в Characters/ должны появиться скелетные меши (~13 файлов)
  4. Потом привязать меш к FighterCharacter

### Баги / известные проблемы
- Геймпад (DualSense) не определяется — отложено
- Манекен UE5 не загрузился (неверный путь) — заменён на цилиндры
- P2 движение не работает через AddMovementInput (не-possessed) — фикс через AddActorWorldOffset

### Файлы проекта
- `FighterCharacter.h/.cpp` — персонаж, ввод, удары, блок, P2 raw input, кровь
- `FighterAnimInstance.h/.cpp` — анимационный C++ класс (пока не используется)
- `HealthComponent.h/.cpp` — здоровье + выносливость
- `FightGameMode.h/.cpp` — спавн, камера, октагон, авто-импорт FBX, KO
- `FightHUD.h/.cpp` — полоски HP/Stamina + KO текст
- `MainPlayerController.h/.cpp` — заглушка
- `Config/` — DefaultInput.ini, DefaultEngine.ini, DefaultEditorPerProjectUserSettings.ini
- `UFCLite.Build.cs` — deps: Slate, SlateCore, UnrealEd (editor)
- `fbx/` — 13 FBX файлов из Mixamo

### Начать следующую сессию
1. `git pull`
2. Удалить битые ассеты из Content/Characters/ в UE5
3. Нажать Play — импорт FBX
4. Если импорт ок → читай дальше по чату
