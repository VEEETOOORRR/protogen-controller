import argparse
import sys
from PIL import Image

def processar_submatriz(img, start_x, start_y, width, height=8):
    """
    Extrai um bloco da imagem e o converte em um array de Matriz8x8 (8 bytes por matriz).
    """
    num_matrizes = width // 8
    matrizes_bytes = []

    for m in range(num_matrizes):
        bytes_matriz = []
        offset_x = start_x + (m * 8)
        
        for y in range(height):
            byte_val = 0
            for x in range(8):
                pixel = img.getpixel((offset_x + x, start_y + y))
                is_on = 1 if (isinstance(pixel, int) and pixel > 128) or (isinstance(pixel, tuple) and pixel[0] > 128) else 0
                
                if is_on:
                    byte_val |= (1 << x)
            
            bytes_matriz.append(f"0x{byte_val:02X}")
        matrizes_bytes.append("{" + ", ".join(bytes_matriz) + "}")
    
    return matrizes_bytes

def gerar_codigo(caminho_imagem, nome_base, tipo):
    try:
        img = Image.open(caminho_imagem).convert("L")
    except Exception as e:
        print(f"// Erro ao abrir imagem: {e}")
        return

    largura, altura = img.size

    if altura % 8 != 0:
        print(f"// ERRO: A altura da imagem ({altura}px) deve ser múltiplo de 8.")
        return

    qtd_frames = altura // 8

    # Define a estrutura de blocos para cada modo: (Nome, Offset_X, Largura)
    if tipo == "todos":
        largura_esperada = 112
        secoes = [
            ("BocaDireita",   0,  32),
            ("OlhoDireito",   32, 16),
            ("NarizDireito",  48, 8),
            ("NarizEsquerdo", 56, 8),
            ("OlhoEsquerdo",  64, 16),
            ("BocaEsquerda",  80, 32),


        ]
    elif tipo == "boca":
        largura_esperada = 64
        secoes = [
            ("BocaDireita",  0,  32),
            ("BocaEsquerda", 32, 32),
        ]
    elif tipo == "olho":
        largura_esperada = 32
        secoes = [
            ("OlhoDireito",  0,  16),
            ("OlhoEsquerdo", 16, 16),
        ]
    elif tipo == "nariz":
        largura_esperada = 16
        secoes = [
            ("NarizDireito",  0, 8),
            ("NarizEsquerdo", 8, 8),
        ]

    if largura != largura_esperada:
        print(f"// AVISO: Para o modo '{tipo}', a largura esperada era {largura_esperada}px, mas a imagem tem {largura}px.")

    print(f"// ========================================================")
    print(f"// ARQUIVO: {caminho_imagem}")
    print(f"// MODO: {tipo.upper()} | FRAMES ENCONTRADOS: {qtd_frames}")
    print(f"// ========================================================\n")

    for frame in range(qtd_frames):
        start_y = frame * 8
        sufixo_frame = f"_FRAME_{frame}" if qtd_frames > 1 else ""

        print(f"// --- FRAME {frame} (Y: {start_y} a {start_y + 7}) ---")
        
        for nome_secao, start_x, width in secoes:
            matrizes = processar_submatriz(img, start_x, start_y, width)
            qtd_matrizes = width // 8
            
            nome_constante = f"{nome_base}_{nome_secao.upper()}{sufixo_frame}"
            print(f"// {nome_secao} ({width}x8 -> {qtd_matrizes} matrizes)")
            print(f"const Matriz8x8 {nome_constante}[{qtd_matrizes}] = {{")
            for m in matrizes:
                print(f"    {m},")
            print("};\n")

def main():
    parser = argparse.ArgumentParser(description="Converte bitmaps para byte arrays em C/C++ (ESP-IDF) para MAX7219.")
    parser.add_argument("imagem", help="Caminho do arquivo de imagem (.png, .bmp, .jpg)")
    parser.add_argument("-n", "--nome", default="EXPRESSAO", help="Nome base da constante em C (Padrão: EXPRESSAO)")
    parser.add_argument("-t", "--tipo", choices=["todos", "boca", "olho", "nariz"], default="todos", 
                        help="Define quais componentes estão presentes no bitmap (Padrão: todos)")

    args = parser.parse_args()
    gerar_codigo(args.imagem, args.nome, args.tipo)

if __name__ == "__main__":
    main()