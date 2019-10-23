import java.util.concurrent.*;
import java.util.ArrayList;

public class Quicksort extends RecursiveAction{

	static final int SIZE = 10;
    static final int THRESHOLD = 2;

    private int start;
    private int end;
    private int[] array;

	public Quicksort(int start, int end, int[] array) {
        this.start = start;
        this.end = end;
        this.array = array;
    }


	protected void compute(){
        if (end - start <= 0) return;
		if (end - start < THRESHOLD) {
			int tmp;
            for (int i = start; i < end; i++){
            	for (int j = i + 1; j <= end; j++){
            		if (array[i] > array[j]){
            			tmp = array[i];
            			array[i] = array[j];
            			array[j] = tmp;
            		}
            	}
            }
        }
        else {
            int pivot = array[start];
            int p_low = start, p_high = end;
            do {
                while (p_low < p_high && array[p_high] >= pivot) p_high --;
                if (p_low < p_high) {
                    array[p_low] = array[p_high];
                    p_low ++;
                }
                while (p_low < p_high && array[p_low] <= pivot) p_low ++;
                if (p_low < p_high) {
                    array[p_high] = array[p_low];
                    p_high --;
                }
            } while (p_low != p_high);
            array[p_low] = pivot;
        	Quicksort leftTask = new Quicksort(start, p_low - 1, array);
        	Quicksort rightTask = new Quicksort(p_low + 1, end, array);
        	leftTask.fork();
        	rightTask.fork();
        	leftTask.join();
        	rightTask.join();
        }
	}

	public static void main(String[] args){
		ForkJoinPool pool = new ForkJoinPool();
		int[] array = new int[SIZE];

		// create SIZE random integers between 0 and 9
		java.util.Random rand = new java.util.Random();

		for (int i = 0; i < SIZE; i++) {
			array[i] = rand.nextInt(100);
		}	

		System.out.print("Unsorted: ");
		for (int i = 0; i < SIZE; i++) {
			System.out.print(array[i] + " ");
		}
		System.out.println("");
		
		// use fork-join parallelism to sum the array
		Quicksort task = new Quicksort(0, SIZE-1, array);

		pool.invoke(task);

		System.out.print("Sorted:   ");
		for (int i = 0; i < SIZE; i++) {
			System.out.print(array[i] + " ");
		}
		System.out.println("");
	}
}