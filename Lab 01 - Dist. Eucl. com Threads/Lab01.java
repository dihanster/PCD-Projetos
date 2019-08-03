import java.util.Random;

public class Lab01 implements Runnable {
    // VARIÁVEIS GLOBAIS
    private static int N = 10000000;  // 10⁵ ou 10⁷
    private static int MAX_THREADS = 2;  // número de threads (1, 2, 4 ou 8)

    private static double r1[] = new double[N];  // vetor R1 (de 0 a 100)
    private static double r2[] = new double[N];  // vetor R2 (de 0 a 100)
    private static double theta1[] = new double[N];  // vetor Theta 1 (de 0 a 360)
    private static double theta2[] = new double[N];  // vetor Theta 2 (de 0 a 360)
    private static double distancias[] = new double[N];  // vetor para salvar as distâncias calculadas
    private static double maximos[] = new double[MAX_THREADS];  // vetor para máximos locais

    private long ID_Thread;  // ID temporário da Thread (usamos para limitar a área da memória compartilhada)
    private Boolean calcMaximo;  // auxiliar para dizer se deve calcular o máximo ou não

    // CONSTRUTOR
    public Lab01(long ID, Boolean calcMax) {
        ID_Thread = ID;
        calcMaximo = calcMax;  // se for FALSE, ele calcula as distâncias, e se for TRUE, calcula o máximo local
    }

    // MÉTODO DA THREAD
    public void run() {
        // calcular distâncias entre cada par de pontos
        if (!calcMaximo) {
            for (long i = ID_Thread; i < ((1 + ID_Thread) * (N / MAX_THREADS)); i++) {
                distancias[(int) i] = Math.sqrt(
                        Math.pow(r1[(int) i], 2)
                                + Math.pow(r2[(int) i], 2)
                                - 2 * r1[(int) i] * r2[(int) i]
                                * Math.cos(Math.toRadians(theta1[(int) i] - theta2[(int) i]))
                );
            }
        }

        // calcular máximos locais
        else {
            double maximoLocal = distancias[(int) ID_Thread];
            for (long i = ID_Thread; i < ((1 + ID_Thread) * (N / MAX_THREADS)); i++)
                if (distancias[(int) i] > maximoLocal)
                    maximoLocal = distancias[(int) i];
            maximos[(int) ID_Thread] = maximoLocal;
        }
    }

    // FUNÇÃO MAIN
    public static void main(String[] args) {
        // VARIÁVEIS
        Random genRand = new Random();  // gerador de números aleatórios
        Lab01[] lab;  // vetor de instâncias da classe
        Thread[] threads;  // vetor de threads

        lab = new Lab01[MAX_THREADS];
        threads = new Thread[MAX_THREADS];

        // INICIAR TEMPO DO PROGRAMA
        System.out.println("INÍCIO DO PROGRAMA");
        long inicioGeral = System.currentTimeMillis();  // marcador de início para o programa inteiro

        // GERAR VALORES ALEATÓRIOS
        for (int i = 0; i < N; i++) {
            r1[i] = 100 * genRand.nextDouble();
            r2[i] = 100 * genRand.nextDouble();
            theta1[i] = 360 * genRand.nextDouble();
            theta2[i] = 360 * genRand.nextDouble();
        }

        // THREADS PARA CALCULAR DISTÂNCIAS
        long inicioCalcDist = System.currentTimeMillis();

        for (int j = 0; j < MAX_THREADS; j++) {
            lab[j] = new Lab01((long) j, Boolean.FALSE);
            threads[j] = new Thread(lab[j]);
            threads[j].start();
        }

        for (int k = 0; k < MAX_THREADS; k++) {
            try {
                threads[k].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        long tempoCalcDist = System.currentTimeMillis() - inicioCalcDist;
        System.out.println("Distâncias Finalizadas");
        System.out.println("Tempo para Distâncias: " + tempoCalcDist + " milissegundos.");

        // THREADS PARA CALCULAR MÁXIMO GLOBAL
        long inicioCalcMaximo = System.currentTimeMillis();

        for (int j = 0; j < MAX_THREADS; j++) {
            lab[j] = new Lab01((long) j, Boolean.TRUE);
            threads[j] = new Thread(lab[j]);
            threads[j].start();
        }

        double maximoGlobal = distancias[0];

        for (int k = 0; k < MAX_THREADS; k++) {
            try {
                threads[k].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int m = 0; m < MAX_THREADS; m++)
            if (maximos[m] > maximoGlobal)
                maximoGlobal = maximos[m];

        long tempoCalcMaximo = System.currentTimeMillis() - inicioCalcMaximo;
        System.out.println("Máximos Finalizados");
        System.out.println("Máximo Global = " + maximoGlobal);
        System.out.println("Tempo para Máximos: " + tempoCalcMaximo + " milissegundos.");

        // CALCULAR TEMPO DO PROGRAMA INTEIRO
        long tempoGeral = System.currentTimeMillis() - inicioGeral;  // final do programa inteiro
        System.out.println("Tempo Geral: " + tempoGeral + " milissegundos.");
        System.out.println("FIM DO PROGRAMA");
    }
}