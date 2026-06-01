#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadra.h"
#include "svg.h"



#define OPACITY 0.5
#define DEFAULT_WIDTH 1.5
#define DEFAULT_FONT "Arial"
#define DEFAULT_WEIGHT "normal"


static void insert_text(FILE *file_name, double x, double y, char* texto, char *corborda, char*corpreench, char* tamanho, char* anchor)
{
    fprintf(file_name, "\t<text x=\"%lf\" y=\"%lf\" stroke=\"%s\" fill=\"%s\" font-family=\"%s\" font-weight=\"%s\" font-size=\"%spt\" text-anchor=\"%s\">%s</text>\n",
       x, y, corborda, corpreench, DEFAULT_FONT, DEFAULT_WEIGHT, tamanho, anchor, texto);
}

static void insert_square(FILE *file_name, double x, double y, double w, double h, char* corpreench, char* corborda)
{
    fprintf(file_name, "\t<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" stroke=\"%s\" fill=\"%s\" opacity=\"%lf\" stroke-width=\"%lf\" />\n",
	    x, y, w, h, corborda, corpreench, OPACITY, DEFAULT_WIDTH);
}

static void insert_circle(FILE *file_name, double x, double y, double r, char* corpreench, char* corborda)
{
    fprintf(file_name, " <circle cx=\"%lf\" cy=\"%lf\" r=\"%lf\" stroke=\"%s\" fill=\"%s\" opacity=\"%lf\" stroke-width=\"%lf\" />\n",
    x, y, r, corpreench, corborda, OPACITY, DEFAULT_WIDTH);
}

static void callback_inserir_quadra(void* item, void* aux) 
{
    Quadra r = (Quadra)item;
    FILE* arquivo = (FILE*)aux; 
    
    svgInsertQuadra(arquivo, r);
}



FILE* startSVG(const char* file_path) {
	FILE* svg = fopen(file_path, "w");
	if (svg == NULL) {
		perror("Erro: Arquivo NULL.");
		return NULL;
	}

     fprintf(svg, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
     fprintf(svg, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"25000\" height=\"25000\">\n");

 

	return svg;
}

void stopSVG(FILE *file_name)
{
    if (file_name == NULL) return;


    fprintf(file_name, "</svg>\n");

    fclose(file_name);
}



void svgInsertQuadra(FILE *file_name, Quadra r)
{
    fprintf(file_name, "\t<rect id=\"%s\" x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" fill=\"%s\" stroke=\"%s\" stroke-width=\"%lf\" fill-opacity=\"%lf\" />\n",
	    quadraGetCep(r), quadraGetCoordX(r), quadraGetCoordY(r), quadraGetWidth(r), quadraGetHeight(r), quadraGetCorPreench(r), quadraGetCorBorda(r), quadraGetBordaWidth(r), 0.5);
}

void svgMarcaRedCross(FILE *file_name, double x, double y)
{
    insert_text(file_name, x, y, "†", "red", "red", "12", "middle");
}

void svgMarcaBlackCircle(FILE *file_name, double x, double y)
{
    insert_circle(file_name, x, y, 5, "black", "black");
}

void svgMarcaRedSquare(FILE *file_name, double x, double y, char* cpf)
{
    insert_square(file_name, x, y, 5, 5, "red", "red");
    insert_text(file_name, x + 5, y + 7, cpf, "white", "white", "4", "middle");
}

void svgMarcaCep(FILE *svg, Quadra q)
{
    // Canto superior esquerdo = x + pequeno offset, y + pequeno offset
    double x = quadraGetCoordX(q) + 2.0;
    double y = quadraGetCoordY(q) + 8.0;

    insert_text(svg, x, y, quadraGetCep(q), "black", "black", "6", "start");
}

void svgMarcaContagemFace(FILE *svg, Quadra q, char face, int contagem)
{
    double x = quadraGetCoordX(q);
    double y = quadraGetCoordY(q);
    double w = quadraGetWidth(q);
    double h = quadraGetHeight(q);
    char texto[16];
    snprintf(texto, sizeof(texto), "%d", contagem);

    // Posiciona próximo ao limite de cada face
    switch(face) {
        case 'N': insert_text(svg, x + w/2, y + 10,     texto, "blue", "blue", "8", "middle"); break;
        case 'S': insert_text(svg, x + w/2, y + h - 3,  texto, "blue", "blue", "8", "middle"); break;
        case 'L': insert_text(svg, x + 3,   y + h/2,    texto, "blue", "blue", "8", "start");  break;
        case 'O': insert_text(svg, x + w-3, y + h/2,    texto, "blue", "blue", "8", "end");    break;
    }
}

void svgMarcaContagemTotal(FILE *svg, Quadra q, int total)
{
    double x = quadraGetCoordX(q) + quadraGetWidth(q)  / 2;
    double y = quadraGetCoordY(q) + quadraGetHeight(q) / 2;
    char texto[16];
    snprintf(texto, sizeof(texto), "%d", total);
    insert_text(svg, x, y, texto, "black", "black", "10", "middle");
}
