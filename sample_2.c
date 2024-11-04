#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <gmp.h>

// Função para calcular o inverso modular usando a GMP
void inverso(mpz_t resultado, mpz_t a, mpz_t b)
{
    mpz_invert(resultado, a, b); // calcula o inverso de a módulo b e armazena em resultado
}

int tamanho(char controle[], int x)
{
    int n = 1;
    for (int i = 0; i < x; i++)
    {
        if (controle[i] == ' ')
            n++;
    }
    return n;
}

void conversao(mpz_t cifra[], char mensagem[], int tam)
{
    int ascii[tam];
    int i;

    for (i = 0; mensagem[i] != '\n' && mensagem[i] != '\0'; i++)
    {
        ascii[i] = mensagem[i];
    }

    mpz_t n, e, base, pot;
    mpz_inits(n, e, base, pot, NULL);

    printf("\nDigite a chave pública:\n\n");
    printf("n: ");
    gmp_scanf("%Zd", n);
    printf("e: ");
    gmp_scanf("%Zd", e);

    for (i = 0; i < tam - 1; i++)
    {
        mpz_set_ui(base, ascii[i]);
        mpz_powm(cifra[i], base, e, n); // cifra[i] = base^e mod n
    }

    mpz_clears(n, e, base, pot, NULL);
}

void desencriptando()
{
    FILE *cripto = fopen("mensagem.txt", "r");

    if (cripto == NULL)
    {
        printf("Arquivo de mensagem não encontrado!\n");
        return;
    }

    printf("Arquivo de mensagem encontrado!\n");

    char controle[1000000];
    fgets(controle, 1000000, cripto);
    fclose(cripto);

    int x = strlen(controle);
    int n = tamanho(controle, x);
    mpz_t cifra[n];

    for (int i = 0; i < n; i++)
    {
        mpz_init(cifra[i]);
    }

    int j = 0;
    for (int i = 0; i < n && controle[j] != '\0'; i++)
    {
        if (isdigit(controle[j]))
        {
            mpz_set_ui(cifra[i], 0);
            while (isdigit(controle[j]))
            {
                mpz_mul_ui(cifra[i], cifra[i], 10);
                mpz_add_ui(cifra[i], cifra[i], controle[j] - '0');
                j++;
            }
        }
        else
        {
            j++;
            i--;
        }
    }

    mpz_t e, p, q, n_value, phi, d, caractere;
    mpz_inits(e, p, q, n_value, phi, d, caractere, NULL);

    printf("\nDigite os valores de...\n");
    printf("e: ");
    gmp_scanf("%Zd", e);
    printf("p: ");
    gmp_scanf("%Zd", p);
    printf("q: ");
    gmp_scanf("%Zd", q);

    mpz_mul(n_value, p, q);
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    inverso(d, e, phi);

    FILE *mensagem = fopen("mensagem_desencriptada.txt", "w");

    for (int i = 0; i < n; i++)
    {
        mpz_powm(caractere, cifra[i], d, n_value); // caractere = cifra[i]^d mod n_value
        char c = mpz_get_ui(caractere);
        fprintf(mensagem, "%c", c);
    }

    fclose(mensagem);

    printf("\nMENSAGEM DESENCRIPTADA COM SUCESSO! Verifique o arquivo 'mensagem_desencriptada.txt'.\n\n");

    mpz_clears(e, p, q, n_value, phi, d, caractere, NULL);
    for (int i = 0; i < n; i++)
    {
        mpz_clear(cifra[i]);
    }
}

void encriptando()
{
    char mensagem[1000000];

    printf("Digite a mensagem:\n");
    scanf(" ");
    fgets(mensagem, 1000000, stdin);

    int tam = strlen(mensagem);
    mpz_t cifra[tam];

    for (int i = 0; i < tam; i++)
    {
        mpz_init(cifra[i]);
    }

    conversao(cifra, mensagem, tam);

    FILE *cripto = fopen("mensagem.txt", "w");

    for (int i = 0; mpz_cmp_ui(cifra[i], 0) != 0; i++)
    {
        gmp_fprintf(cripto, "%Zd ", cifra[i]);
    }

    fclose(cripto);

    printf("\nMENSAGEM ENCRIPTADA COM SUCESSO\n\n");

    for (int i = 0; i < tam; i++)
    {
        mpz_clear(cifra[i]);
    }
}

void chavepublica()
{
    mpz_t p, q, phi, e, n;
    mpz_inits(p, q, phi, e, n, NULL);

    printf("\nDigite dois números primos\n\n");
    printf("p: ");
    gmp_scanf("%Zd", p);

    while (!mpz_probab_prime_p(p, 25))
    {
        gmp_printf("\nO número %Zd não é primo! Tente novamente.\n", p);
        printf("p: ");
        gmp_scanf("%Zd", p);
    }

    printf("q: ");
    gmp_scanf("%Zd", q);

    while (!mpz_probab_prime_p(q, 25))
    {
        gmp_printf("\nO número %Zd não é primo! Tente novamente.\n", q);
        printf("q: ");
        gmp_scanf("%Zd", q);
    }

    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    printf("\nDigite um número relativamente primo a ");
    gmp_printf("%Zd:\n\ne: ", phi);
    gmp_scanf("%Zd", e);

    while (mpz_gcd_ui(NULL, phi, mpz_get_ui(e)) != 1)
    {
        gmp_printf("\n%Zd e %Zd não são coprimos! Tente novamente.\n\n", phi, e);
        printf("Digite um número relativamente primo a ");
        gmp_printf("%Zd:\ne: ", phi);
        gmp_scanf("%Zd", e);
    }

    mpz_add_ui(p, p, 1);
    mpz_add_ui(q, q, 1);
    mpz_mul(n, p, q);

    FILE *chavepublica = fopen("chave.txt", "w");
    gmp_fprintf(chavepublica, "n: %Zd\ne: %Zd", n, e);
    fclose(chavepublica);

    printf("\nCHAVE PÚBLICA GERADA COM SUCESSO!\n\n");

    mpz_clears(p, q, phi, e, n, NULL);
}

int main()
{
    printf("Escolha uma das opções abaixo:\n\n");
    printf("1 - Gerar chave pública\n");
    printf("2 - Encriptar mensagem\n");
    printf("3 - Desencriptar mensagem\n\n");

    int x;
    printf("Digite a opção escolhida: ");
    scanf("%d", &x);

    while (x < 1 || x > 3)
    {
        printf("\nNúmero digitado inválido! Tente novamente.\n\n");
        printf("Digite a opção escolhida: ");
        scanf("%d", &x);
    }

    if (x == 1)
    {
        printf("\nGERAR CHAVE PÚBLICA\n");
        chavepublica();
    }
    else if (x == 2)
    {
        printf("\nENCRIPTAR MENSAGEM\n\n");
        encriptando();
    }
    else
    {
        printf("\nDESENCRIPTAR MENSAGEM\n\n");
        desencriptando();
    }

    return 0;
}