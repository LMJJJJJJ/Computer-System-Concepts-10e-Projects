import java.util.concurrent.*;
import java.util.ArrayList;

public class Mergesort extends RecursiveAction{

	static final int SIZE = 10;
    static final int THRESHOLD = 2;

    private int start;
    private int end;
    private int[] array;

	public Mergesort(int start, int end, int[] array) {
        this.start = start;
        this.end = end;
        this.array = array;
    }


	protected void compute(){
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
        	int mid = (start + end) / 2;
        	Mergesort leftTask = new Mergesort(start, mid, array);
        	Mergesort rightTask = new Mergesort(mid + 1, end, array);
        	leftTask.fork();
        	rightTask.fork();
        	leftTask.join();
        	rightTask.join();
        	ArrayList<Integer> tmp = new ArrayList<Integer>();
        	int p_left = start, p_right = mid + 1;
        	while (p_left <= mid && p_right <= end){
        		if (array[p_left] < array[p_right]){
        			tmp.add(array[p_left]);
        			p_left += 1;
        		}
        		else {
        			tmp.add(array[p_right]);
        			p_right += 1;
        		}
        	}
        	while (p_left <= mid){
        		tmp.add(array[p_left]);
        		p_left += 1;
        	}
        	while (p_right <= end){
        		tmp.add(array[p_right]);
        		p_right += 1;
        	}
        	for (int i = 0; i < tmp.size(); i++){
        		array[i + start] = tmp.get(i);
        	}
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
		Mergesort task = new Mergesort(0, SIZE-1, array);

		pool.invoke(task);

		System.out.print("Sorted:   ");
		for (int i = 0; i < SIZE; i++) {
			System.out.print(array[i] + " ");
		}
		System.out.println("");
	}
}