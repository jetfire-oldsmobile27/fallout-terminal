from PIL import Image, ImageDraw, ImageFont

# Размер иконки (256x256) и координаты центра
size = (256, 256)
center = (size[0] // 2, size[1] // 2)

# Создаём изображение с прозрачным фоном
img = Image.new("RGBA", size, (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# Рисуем круг – фон логотипа Vault‑Tec
radius = 100
bbox = [center[0] - radius, center[1] - radius, center[0] + radius, center[1] + radius]
# Заливка – глубокий синий, обводка – белая
draw.ellipse(bbox, fill=(0, 85, 170, 255), outline=(255, 255, 255, 255), width=8)

# Рисуем стилизованное "V"
# Координаты подбираются так, чтобы форма напоминала оригинальный логотип:
v_points = [
    (center[0], center[1] - 50),      # верхняя точка (вершина V)
    (center[0] - 50, center[1] + 50),   # левый внешний угол
    (center[0] - 30, center[1] + 50),   # левый внутренний угол
    (center[0], center[1] + 20),        # нижняя точка (вогнутость V)
    (center[0] + 30, center[1] + 50),   # правый внутренний угол
    (center[0] + 50, center[1] + 50)    # правый внешний угол
]
draw.polygon(v_points, fill=(255, 255, 255, 255))

# Добавляем отсылку к консоли PowerShell – текст ">_"
try:
    font = ImageFont.truetype("arial.ttf", 24)
except IOError:
    font = ImageFont.load_default()

ps_text = ">_"
# Вычисляем размеры текста для корректного позиционирования
bbox_text = draw.textbbox((0, 0), ps_text, font=font)
text_width = bbox_text[2] - bbox_text[0]
text_height = bbox_text[3] - bbox_text[1]
# Размещаем текст в правом нижнем углу, вне круга
text_position = (size[0] - text_width - 10, size[1] - text_height - 10)
draw.text(text_position, ps_text, font=font, fill=(255, 255, 255, 255))

# Сохраняем изображение в формате ICO
img.save("Vault-Tec.ico", format="ICO")
