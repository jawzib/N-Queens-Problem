import pygame
import argparse

# Create parser to get file
parser = argparse.ArgumentParser()

parser.add_argument(
    "file",
    help="File with board to be read in, refer to Program Instructions.pdf for file format",
)

# Get command line args
args = parser.parse_args()

# open text file in read mode
text_file = open(args.file, "r")

# read whole file to a string
raw = text_file.readline()

# close file
text_file.close()

print(raw)

data = raw.strip().split(" ")
dim = len(data)

cell_size = 1
if dim == 10:
    cell_size = 50
if dim == 100:
    cell_size = 25
if dim == 1000:
    cell_size = 10


(width, height) = (dim * cell_size, dim * cell_size)
screen = pygame.display.set_mode((width, height))

screen.fill((255, 255, 255))

for i in range(dim):
    color = (255, 0, 0)
    pygame.draw.rect(
        screen,
        color,
        pygame.Rect(i * cell_size, int(data[i]) * cell_size, cell_size, cell_size),
    )

if dim >= 10:
    for i in range(dim):
        color = (0, 0, 0)
        pygame.draw.line(screen, color, (i * cell_size, 0), (i * cell_size, height))
        pygame.draw.line(screen, color, (0, i * cell_size), (width, i * cell_size))


pygame.display.flip()


running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
