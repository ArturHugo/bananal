/*****************************************************************************************/
/** Autor: Felipe Kenzo Shiraishi                                                       **/
/** Orientador: Professor Edson Gomi Satoshi                                            **/
/** Escola: Escola Politécnica de Engenharia da USP                                     **/
/** Parte: Gerador de Redes Neurais Artificiais                                         **/
/** Projeto: Gerador de Arquitetura de Redes Neurais Artificiais por Algoritmo Genético **/
/** Data de Término: 19/02/2018                                                         **/
/** Linguagem: C                                                                        **/
/*****************************************************************************************/

/**INCLUSÃO DE BIBLIOTECAS**/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**DEFINIÇÃO DE ESTRUTURAS DE DADOS**/
typedef struct perceptron
{
	int id;
	int idcamada;
	double *peso;
	int entradas;
	double saida;
	double *corretores;
} neuron;

typedef struct camada
{
	int id;
	int numperps;
	neuron **percept;
} cmd;

typedef struct rede
{
	cmd **camadas;
} net;


/**DEFINIÇÃO DE FUNÇÕES AUXILIARES**/
double logistica(double x)
{
	return 1.0 / (1.0 + pow(2.718281, x));
}

double derivlogistica(double x)
{
	return logistica(x) * (1 - logistica(x));
}

double comblinear(double *entradas, double *pesos, int numentradas)
{
	double resultados;
	int i;

	resultados = 0;
	for (i = 0; i < numentradas; i++)
	{
		resultados += entradas[i] * pesos[i];
	}
	return resultados;
}

void alimentarneuron(double *entradas, neuron *perp)
{
	perp->saida = logistica(comblinear(entradas, perp->peso, perp->entradas));
}

void feedcamada(double *entradas, cmd *camada)
{
	int i;

	for (i = 0; i < camada->numperps; i++)
	{
		alimentarneuron(entradas, camada->percept[i]);
	}
}

double * getcmdsaida(cmd *camada)
{
	double *output;
	int i;

	output = malloc(camada->numperps * sizeof(double));

	for (i = 0; i < camada->numperps; i++)
	{
		output[i] = camada->percept[i]->saida;
	}

	return output;
}

void feedforward(double *entradas, int cmdout, cmd **camadas)
{
	int i;

	/**FEED DA ENTRADA**/
	for (i = 0; i < camadas[0]->numperps; i++)
	{
		alimentarneuron(&entradas[i], camadas[0]->percept[i]);
	}

	/**FEED DAS CAMADAS SEGUINTES**/
	for (i = 1; i <= cmdout; i++)
	{
		feedcamada(getcmdsaida(camadas[i - i]), camadas[i]);
	}
}

void atualizarpeso(cmd ** camadas, int numcamadas)
{
	int i, j, k;

	for (i = 1; i < numcamadas; i++)
	{
		for (j = 0; j < camadas[i]->numperps; j++)
		{
			for (k = 0; k < camadas[i - 1]->numperps; k++)
			{
				camadas[i]->percept[j]->peso[k] -= camadas[i]->percept[j]->corretores[i] * getcmdsaida(camadas[i - 1])[k];
			}
		}
	}
}

void calculasinalerro(double deriverro, cmd ** camadas, int numcamadas)
{
	int i, j, k;
	printf("\nproblema na calculasinalerro");
	for (i = 1; i < numcamadas; i++)
	{
		printf("\nDeu problema na iteracao de i = %d", i);
		for (j = 0; j < camadas[i]->numperps; j++)
		{
			printf("\n\tDeu problema na iteracao de j = %d", j);
			for (k = 0; k < camadas[i - 1]->numperps; k++)
			{
				//O PROBLEMA ESTÁ AQUI
				printf("\n\t\tDeu problema na iteracao de k = %d", k);
				camadas[i]->percept[j]->corretores[k] = deriverro * camadas[i]->percept[j]->saida * (1 - camadas[i]->percept[j]->saida);
			}
		}
	}
}

void backpropagation(cmd ** camadas, double * deriverro, int numcamadas)
{
	int i;
	for (i = 0; i < camadas[numcamadas - 1] -> numperps; i++)
	{

		calculasinalerro(deriverro[i], camadas, numcamadas);
		printf("Problema resolvido");
		atualizarpeso(camadas, numcamadas);
	}
}

/**CONCLUIDO**/
void criarTabela(FILE *arquivo, int *linhas, int *colunas)
{
	char charlido;
	//descobrir o numero de colunas
	charlido = fgetc(arquivo);
	*colunas = 0;
	*linhas = 1;
	while (charlido != '\n')
	{
		if (charlido == ';')
		{
			*colunas = *colunas + 1;
		}

		charlido = fgetc(arquivo);
	}
	//descobrir o numero de linhas
	while (charlido != -1)
	{
		if (charlido == '\n')
		{
			*linhas = *linhas + 1;
		}
		charlido = fgetc(arquivo);
	}
}

/**CONCLUIDO**/
void preencherTabela(double **treino, FILE *arquivo)
{
	char charlido;
	double numero;
	int i, j, numeroLido, k, posponto, l;
	double algarismos[100];

	charlido = fgetc(arquivo);
	i = 0;
	j = 0;
	k = 0;
	posponto = -1;
	while (charlido != -1)
	{
		while (charlido != ';' && charlido != '\n' && charlido != -1)
		{
			if (charlido >= 48 && charlido <= 57)
			{
				numeroLido = charlido - 48;
				algarismos[k] = numeroLido;
				k++;
			}
			else if (charlido == ',')
			{
				posponto = k;
				algarismos[k] = -1;
				k++;
			}
			charlido = fgetc(arquivo);
		}
		l = 0;
		numero = 0;
		while (l < k)
		{
			if (l != posponto)
			{
				if (l < posponto)
				{
					numero += algarismos[l] * pow(10, posponto - l - 1);
				}
				else
				{
					numero += algarismos[l] * pow(10, posponto - l);
				}
			}
			l++;
		}
		treino[i][j] = numero;
		if (charlido == ';')
		{
			j++;
		}
		else if (charlido == '\n')
		{
			i++;
			j = 0;
		}
		if (charlido != -1)
		{
			charlido = fgetc(arquivo);
		}
		k = 0;
		posponto = -1;
	}
}

int main()
{
	int camadas, entradas, saidas, i, linhas, colunas, validacao, epocas, j, numPerp, k;
	int *perceptrons;
	double **trainData;
	double *derivadaserroquad;
	char nomeRNA[20];
	cmd **CMDs;
	neuron **NEURONs;

	int acertos;
	double acuracia, respostaaux;
	double *respostas;
	int palpite;

	FILE *arq;
	FILE *netTreinada;

	arq = fopen("data.csv", "r");
	criarTabela(arq, &linhas, &colunas);

	trainData = (double **)malloc(linhas * sizeof(double*));
	for (i = 0; i < linhas; i++)
	{
		trainData[i] = (double *)malloc(colunas * sizeof(double));
	}
	rewind(arq);
	preencherTabela(trainData, arq);
	printf("Quantas camadas ocultas?\n");
	scanf("%d", &camadas);

	printf("\nQuantas entradas?\n");
	scanf("%d", &entradas);

	printf("\nQuantas saidas?\n");
	scanf("%d", &saidas);

	printf("\nQuantas epocas?\n");
	scanf("%d", &epocas);

	printf("\nQuantas amostras de validar?\n");
	scanf("%d", &validacao);

	perceptrons = (int *) malloc(camadas * sizeof(int));
	numPerp = entradas + saidas;

	for (i = 0; i < camadas; i++)
	{
		printf("\nQuantos neuronios na camada %d?", i + 1);
		scanf("%d", &perceptrons[i]);
		numPerp += perceptrons[i];
	}

	system("cls");
	printf("Alocando Memoria\n");
	/**Alocação para a memória da Rede**/
	NEURONs = (neuron **)malloc(numPerp * sizeof(neuron*));
	for (i = 0; i < numPerp; i++)
	{
		NEURONs[i] = (neuron *)malloc(sizeof(neuron));
		NEURONs[i]->id = i;
	}
	camadas += 2;
	CMDs = (cmd**)malloc(camadas * sizeof(cmd*));
	for (i = 0; i < camadas; i++)
	{
		CMDs[i] = (cmd*)malloc(sizeof(cmd));
		CMDs[i]->id = i;
	}
	derivadaserroquad = (double *)malloc(saidas * sizeof(double));
	printf("Memoria Alocada com Sucesso");

	system("cls");
	printf("Criando neurônios das camadas\n");
	/**CRIAÇÃO DA CAMADA DE ENTRADA**/
	CMDs[0]->percept = (neuron**)malloc(entradas * sizeof(neuron*));
	for (i = 0; i < entradas; i++)
	{
		CMDs[0]->percept[i] = (neuron*)malloc(sizeof(neuron));
		CMDs[0]->percept[i] = NEURONs[i];
		CMDs[0]->percept[i]->idcamada = 0;
	}

	k = entradas;
	/**CRIAÇÃO DAS CAMADAS OCULTAS**/
	for (j = 1; j < camadas - 1; j++)
	{
		CMDs[j]->percept = (neuron**)malloc(perceptrons[j - 1] * sizeof(neuron*));
		for (i = 0; i < perceptrons[j - 1]; i++)
		{
			CMDs[j]->percept[i] = (neuron*)malloc(sizeof(neuron));
			CMDs[j]->percept[i] = NEURONs[k];
			CMDs[j]->percept[i]->idcamada = j;
			k++;
		}
	}

	/**CRIAÇÃO DA CAMADA DE SAÍDA**/
	CMDs[j]->percept = (neuron**)malloc(saidas * sizeof(neuron*));
	respostas = (double *)malloc(saidas * sizeof(double));
	for (i = 0; i < saidas; i++)
	{
		CMDs[j]->percept[i] = (neuron*)malloc(sizeof(neuron));
		CMDs[j]->percept[i] = NEURONs[k];
		CMDs[j]->percept[i]->idcamada = j;
		k++;
	}

	printf("Criação de neurônios concluída");

	system("cls");
	printf("Criação dos pesos em processo\n");
	/**Criação de Pesos da Rede**/
	/**PESOS DA ENTRADA**/
	CMDs[0]->numperps = entradas;
	for (i = 0; i < entradas; i++)
	{
		CMDs[0]->percept[i]->peso = malloc(sizeof(double));
		CMDs[0]->percept[i]->peso[0] = 1;
		CMDs[0]->percept[i]->entradas = 1;
		CMDs[0]->percept[i]->corretores = malloc(sizeof(double));
	}
	printf("\tCriado Neuronios da camada de entrada\n");
	/**PESOS DAS CAMADAS OCULTAS**/
	for (i = 1; i < camadas - 1; i++)
	{
		for (j = 0; j < perceptrons[i]; j++)
		{
			CMDs[i]->percept[j]->peso = malloc((CMDs[i - 1]->numperps) * sizeof(double));
			CMDs[i]->percept[j]->corretores = malloc((CMDs[i - 1]->numperps) * sizeof(double));
			for (k = 0; k < CMDs[i - 1]->numperps; k++)
			{
				CMDs[i]->percept[j]->peso[k] = 1;
			}
			CMDs[i]->percept[j]->entradas = CMDs[i - 1]->numperps;
		}
		CMDs[i]->numperps = perceptrons[i - 1];
	}
	printf("\tCriado Neuronios das camadas ocultas\n");
	/**PESOS DAS CAMADAS DE SAÍDA**/
	CMDs[camadas - 1]->numperps = saidas;
	for (i = 0; i < saidas; i++)
	{
		CMDs[camadas - 1]->percept[i]->peso = malloc((CMDs[camadas - 2]->numperps) * sizeof(double));
		CMDs[camadas - 1]->percept[i]->corretores = malloc((CMDs[camadas - 2]->numperps) * sizeof(double));
		CMDs[camadas - 1]->percept[i]->entradas = CMDs[camadas - 2] ->numperps;
		for (j = 0; j < CMDs[camadas - 2]->numperps; j++)
		{
			CMDs[camadas - 1]->percept[i]->peso[j] = 1;
		}
	}
	printf("Pesos Criados com Sucesso");

	system("cls");
	printf("Treinando a rede\n");
	/**Treinar Rede**/
	for (i = 0; i < epocas; i++)
	{
		for (j = 0; j < linhas - validacao; j++)
		{
			feedforward(trainData[j], camadas - 1, CMDs);
			for (k = 0; k < saidas; k++)
			{
				derivadaserroquad[k] = getcmdsaida(CMDs[camadas - 1])[k] - trainData[j][entradas + k];
			}
			printf("OK");
			backpropagation(CMDs, derivadaserroquad, camadas);
			printf("OK");
		}
	}
	printf("Rede criada com sucesso");

	system("cls");
	printf("Testando a rede");
	/**Testar a Rede**/
	acertos = 0;
	acuracia = 0;
	for (i = (linhas - validacao); i < linhas; i++)
	{
		feedforward(trainData[i], camadas - 1, CMDs);
		respostaaux = 0;
		for (j = 0; j < saidas; j++)
		{
			respostas[j] = getcmdsaida(CMDs[camadas - 1])[j];
			if (respostas[j] - respostaaux > 0)
			{
				palpite = j;
			}
			respostaaux = respostas[j];
		}
		if (trainData[i][entradas + palpite] != 0)
		{
			acertos++;
		}
	}
	acuracia = (double)acertos / (double)validacao;
	printf("A rede obteve uma acuracia de %lf acertando %d de %d amostras\n", acuracia, acertos, validacao);

	fclose(arq);

	/**Guardar a Rede**/
	printf("Digite o nome da RNA: ");
	scanf("%s", nomeRNA);
	netTreinada = fopen(nomeRNA, "w");
	//Cabeçalho
	fprintf(netTreinada, ";entradas;pesos;camada\n");
	for (i = 0; i < numPerp; i++)
	{
		//Id do neuronio
		fprintf(netTreinada, "neuron %d;", NEURONs[i]->id);
		//Número de Entradas
		fprintf(netTreinada, "%d;", NEURONs[i]->entradas);
		//Vetor de pesos
		fprintf(netTreinada, "(");
		for (j = 0; j < NEURONs[i]->entradas; j++)
		{
			fprintf(netTreinada, "%lf,", NEURONs[i]->peso[j]);
		}
		fprintf(netTreinada, ");");
		//Em que camada está
		fprintf(netTreinada, "%d\n", NEURONs[i]->idcamada);
	}
	fclose(netTreinada);
	printf("\nSalvamento Concluído. Aperte enter para finalizar.");
	scanf("%d", &i);

	return 0;
}
