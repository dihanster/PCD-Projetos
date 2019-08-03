import java.util.Random;
import java.util.concurrent.Semaphore;

public class Exercicio1 implements Runnable {
    /*variáveis globais*/
    private static int N = 10000000;
    private static int MAX_THREADS = 8;
    private long ID_Thread;   // auxiliar para limitar região do vetor
    private static double PE = 0;   // resultado do produto escalar

    private static double A[] = new double[N];
    private static double B[] = new double[N];

    private Semaphore semaforoSC;

    /*CONSTRUTOR DA CLASSE*/
    public Exercicio1(long ID, Semaphore sem) {
        this.ID_Thread = ID;
        this.semaforoSC = sem;
    }

    /*GERAR VALORES ALEATÓRIOS PARA OS VETORES*/
    public static void gerarAleatorios() {
        Random genAleatorio = new Random();

        for (int i = 0; i < N; i++) {
            A[i] = genAleatorio.nextDouble();
            B[i] = genAleatorio.nextDouble();
        }
    }

    /*MÉTODO DE EXECUÇÃO DA THREAD*/
    public void run() {
        double resultadoParcial = 0;

        for (int i = (int)((ID_Thread * N) / MAX_THREADS); i < ((ID_Thread + 1) * (N / MAX_THREADS)); i++)
            resultadoParcial = resultadoParcial + (A[i] * B[i]);

        try {
            semaforoSC.acquire();
        } catch (InterruptedException ie) {
            System.out.println("Exceção");
        }

        PE += resultadoParcial;
        semaforoSC.release();
    }

    /*MÉTODO PRINCIPAL*/
    public static void main(String[] args) {
        Semaphore semaforoSC = new Semaphore(1, true);
        Exercicio1[] instancias = new Exercicio1[MAX_THREADS];
        Thread[] threads = new Thread[MAX_THREADS];

        gerarAleatorios();

        for (int i = 0; i < MAX_THREADS; i++) {
            instancias[i] = new Exercicio1((long) i, semaforoSC);
            threads[i] = new Thread(instancias[i]);
            threads[i].start();
        }

        for (int j = 0; j < MAX_THREADS; j++) {
            try {
                threads[j].join();
            } catch (InterruptedException ie) {
                ie.printStackTrace();
            }
        }

        System.out.println("Produto Escalar = " + PE);
    }
}
