#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>

#define MAX_USERS 100
#define USERNAME_LEN 50
#define PASSWORD_LEN 50
#define FILENAME "Usuarios.txt"
#define TAM_CODIGO 3

typedef struct {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
} Usuario;

Usuario usuarios[MAX_USERS];
int contagem = 0;

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void desenharJanela(int x, int y, int largura, int altura, const char *titulo) {
    gotoxy(x, y);
    printf("+%s+\n", "------------------------------------------");
    gotoxy(x, y + 1);
    printf("| %-41s|\n", titulo);
    gotoxy(x, y + 2);
    printf("+%s+\n", "------------------------------------------");
}

int validarSenha(const char *senha) {
    int temMaiusculo = 0, temMinusculo = 0, temNumero = 0, temEspecial = 0;
    const char *caracteresEspeciais = "!@#$%¨&*()>?°º][´~;/\\-+.,><";

    for (int i = 0; senha[i] != '\0'; i++) {
        char c = senha[i];
        if (isupper(c)) temMaiusculo = 1;
        else if (islower(c)) temMinusculo = 1;
        else if (isdigit(c)) temNumero = 1;
        else if (strchr(caracteresEspeciais, c) != NULL) temEspecial = 1;
    }

    return temMaiusculo && temMinusculo && temNumero && temEspecial;
}

void criptografar(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] += TAM_CODIGO; 
    }
}

void descriptografar(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] -= TAM_CODIGO; 
    }
}

void salvarUsuarios() {
    FILE *arquivo = fopen(FILENAME, "w");
    if (arquivo) {
        for (int i = 0; i < contagem; i++) {
            criptografar(usuarios[i].username);
            criptografar(usuarios[i].password);
            
            fprintf(arquivo, "%s %s\n", usuarios[i].username, usuarios[i].password);
            
            descriptografar(usuarios[i].username);
            descriptografar(usuarios[i].password);
        }
        fclose(arquivo);
    }
}

void carregarUsuarios() {
    FILE *arquivo = fopen(FILENAME, "r");
    if (arquivo) {
        while (fscanf(arquivo, "%s %s", usuarios[contagem].username, usuarios[contagem].password) == 2) {
            descriptografar(usuarios[contagem].username);
            descriptografar(usuarios[contagem].password);
            contagem++;
        }
        fclose(arquivo);
    }
}

void setColor(int textColor, int bgColor) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)(bgColor << 4 | textColor));
}

void displayMenu(int highlight) {
    system("cls");
    desenharJanela(10, 3, 50, 5, "Menu de Usuários");

    const char *menuOptions[] = {
        "1. Inserir Usuários",
        "2. Editar  Usuários",
        "3. Excluir Usuários",
        "4. Listar  Usuários"
    };

	int colors[] = {2, 1, 4, 6};
    int numOptions = sizeof(menuOptions) / sizeof(menuOptions[0]);

    for (int i = 0; i < numOptions; i++) {
        gotoxy(15, 6 + i * 2);
        if (i == highlight) {
            setColor(15, colors[i]);
        } else {
            setColor(7, 0);
        }
        printf("%s\n", menuOptions[i]);
    }

    setColor(7, 0);
    gotoxy(15, 14);
    printf("Use as setas para navegar e Enter para selecionar.");
}

void adicionarUsuario() {
    system("cls");
    desenharJanela(10, 3, 50, 10, "Adicionar Usuário");

    if (contagem >= MAX_USERS) {
        gotoxy(12, 6);
        printf("Limite de usuários atingido.");
        gotoxy(12, 8);
        printf("Pressione qualquer tecla para voltar.");
        getch();
        return;
    }

    Usuario novoUsuario;
    gotoxy(12, 6);
    printf("Nome de usuário: ");
    scanf("%s", novoUsuario.username);

    while (1) {
        gotoxy(12, 8);
        printf("Senha: ");
        scanf("%s", novoUsuario.password);

        if (!validarSenha(novoUsuario.password)) {
            setColor(12, 0);
            gotoxy(12, 10);
            printf("Error! Senha inválida. Tente novamente.");
            setColor(7, 0);
            gotoxy(12, 8);
            printf("Senha:                  ");
        } else {
            setColor(10, 0);
            gotoxy(12, 10);
            printf("Senha correta!                         ");
            setColor(7, 0);
            break;
        }
    }

    criptografar(novoUsuario.username);
    criptografar(novoUsuario.password);
    usuarios[contagem++] = novoUsuario;

    salvarUsuarios();
    setColor(10, 0); 
    gotoxy(12, 18);
    printf("Usuário adicionado com sucesso.");
    setColor(7, 0); 
    getch();
}

void editarUsuario() {
    system("cls");
    desenharJanela(10, 3, 50, 15, "Modificar Usuário");

    char username[USERNAME_LEN];
    gotoxy(12, 6);
    printf("Digite o nome de usuário a ser alterado: ");
    scanf("%s", username);

    int usuarioIndex = -1;
    for (int i = 0; i < contagem; i++) {
        descriptografar(usuarios[i].username);
        if (strcmp(usuarios[i].username, username) == 0) {
            usuarioIndex = i;
            break;
        }
        criptografar(usuarios[i].username);
    }

    if (usuarioIndex == -1) {
        setColor(12, 0); 
        gotoxy(12, 8);
        printf("Usuário não encontrado.");
        setColor(7, 0); 
        getch();
        return;
    }

    setColor(10, 0); 
    gotoxy(12, 8);
    printf("Usuário encontrado: %s", username);
    setColor(7, 0); 

    int opcao;
    gotoxy(12, 10);
    printf("Deseja alterar: (1) Nome (2) Senha (3) Ambos: ");
    scanf("%d", &opcao);

    if (opcao == 1 || opcao == 3) {
        gotoxy(12, 12);
        printf("Novo nome: ");
        scanf("%s", usuarios[usuarioIndex].username);
    }

    if (opcao == 2 || opcao == 3) {
        while (1) {
            gotoxy(12, 14);
            printf("Nova senha: ");
            scanf("%s", usuarios[usuarioIndex].password);

            if (!validarSenha(usuarios[usuarioIndex].password)) {
                setColor(12, 0); 
                gotoxy(12, 16);
                printf("Error! Senha inválida. Tente novamente.");
                setColor(7, 0); 
                gotoxy(12, 14);
                printf("Nova senha:              "); 
            } else {
                setColor(10, 0); 
                gotoxy(12, 16);
                printf("Senha válida!                          ");
                setColor(7, 0);
                break;
            }
        }
    }

    criptografar(usuarios[usuarioIndex].username);
    criptografar(usuarios[usuarioIndex].password);

    salvarUsuarios();
    setColor(10, 0); 
    gotoxy(12, 18);
    printf("Usuário alterado com sucesso.");
    setColor(7, 0); 
    getch();
}

void excluirUsuario() {
    system("cls");
    desenharJanela(10, 3, 50, 12, "Excluir Usuário");

    char username[USERNAME_LEN];
    gotoxy(12, 6);
    printf("Digite o nome de usuário a ser excluído: ");
    scanf("%s", username);

    int usuarioIndex = -1;
    for (int i = 0; i < contagem; i++) {
        descriptografar(usuarios[i].username);
        if (strcmp(usuarios[i].username, username) == 0) {
            usuarioIndex = i;
            break;
        }
        criptografar(usuarios[i].username);
    }

    if (usuarioIndex == -1) {
        setColor(12, 0); 
        gotoxy(12, 8);
        printf("Usuário não encontrado.");
        setColor(7, 0); 
        getch();
        return;
    }

    setColor(10, 0); 
    gotoxy(12, 8);
    printf("Usuário encontrado: %s", username);
    setColor(7, 0);

    gotoxy(12, 10);
    printf("Deseja realmente excluir? (s/n): ");
    char confirma;
    scanf(" %c", &confirma);

    if (confirma == 's' || confirma == 'S') {
        for (int i = usuarioIndex; i < contagem - 1; i++) {
            usuarios[i] = usuarios[i + 1];
        }
        contagem--;
        salvarUsuarios();

        setColor(10, 0); 
        gotoxy(12, 12);
        printf("Usuário excluído com sucesso.");
        setColor(7, 0); 
    } else {
        setColor(14, 0); 
        gotoxy(12, 12);
        printf("Operação cancelada.");
        setColor(7, 0); 
    }
    getch();
}

void listarUsuarios() {
    int modoVisualizacao = 0; 
    char tecla;

    while (1) {
        system("cls");
        desenharJanela(10, 3, 50, 10 + contagem * 2, "Lista de Usuários");

        if (contagem == 0) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); 
            gotoxy(12, 6);
            printf("Nenhum usuário encontrado.");
            
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        } else {
            for (int i = 0; i < contagem; i++) {
                char nome[USERNAME_LEN];
                char senha[PASSWORD_LEN];

                if (modoVisualizacao == 0) {
                    descriptografar(usuarios[i].username);
                    strcpy(nome, usuarios[i].username);
                    strcpy(senha, "*******");
                    criptografar(usuarios[i].username);
                } else if (modoVisualizacao == 1) {
                    strcpy(nome, usuarios[i].username);
                    strcpy(senha, usuarios[i].password);
                } else if (modoVisualizacao == 2) {
                    descriptografar(usuarios[i].username);
                    descriptografar(usuarios[i].password);
                    strcpy(nome, usuarios[i].username);
                    strcpy(senha, usuarios[i].password);
                    criptografar(usuarios[i].username);
                    criptografar(usuarios[i].password);
                }

                gotoxy(12, 6 + i * 2);
                printf("Nome: %s, Senha: %s", nome, senha);
            }

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); 
            gotoxy(12, 6 + contagem * 2 + 2);
            printf("Pressione F2 (Senha oculta), F3 (Criptografada), F4 (Descriptografada), ESC (Sair)");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        }

        tecla = getch();
        if (tecla == 0 || tecla == 224) { 
            tecla = getch();
            if (tecla == 60) { 
                modoVisualizacao = 0;
            } else if (tecla == 61) { 
                modoVisualizacao = 1;
            } else if (tecla == 62) { 
                modoVisualizacao = 2;
            }
        } else if (tecla == 27) { 
            break;
        }
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    carregarUsuarios();

    int highlight = 0;
    int tecla;
    do {
        displayMenu(highlight);
        tecla = getch();

        if (tecla == 224) {
            tecla = getch();
            switch (tecla) {
                case 72:
                    highlight = (highlight - 1 + 4) % 4;
                    break;
                case 80:
                    highlight = (highlight + 1) % 4;
                    break;
            }
        } else if (tecla == 13) {
            switch (highlight) {
                case 0: adicionarUsuario(); break;
                case 1: editarUsuario(); break;
                case 2: excluirUsuario(); break;
                case 3: listarUsuarios(); break;
            }
        }
    } while (tecla != 27);

    return 0;
}
