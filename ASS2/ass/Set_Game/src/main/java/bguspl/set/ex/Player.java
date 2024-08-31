package bguspl.set.ex;
import java.util.Objects;
import java.util.Random;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.LinkedBlockingQueue;

import bguspl.set.Env;
import bguspl.set.Util;

/**
 * This class manages the players' threads and data
 *
 * @inv id >= 0
 * @inv score >= 0
 */
public class Player implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;

    /**
     * The id of the player (starting from 0).
     */
    public final int id;

    /**
     * The thread representing the current player.
     */
    private Thread playerThread;

    /**
     * The thread of the AI (computer) player (an additional thread used to generate key presses).
     */
    private Thread aiThread;

    /**
     * True iff the player is human (not a computer player).
     */
    private final boolean human;

    /**
     * True iff game should be terminated.
     */
    private volatile boolean terminate;

    /**
     * The current score of the player.
     */
    private int score;

    //-------------------------------------------
    protected boolean isPoint;
    protected boolean isPenalty;
    protected boolean dealerResponded ;
    public Object lock ;




//    private final Dealer dealer ;

    /**
     * The class constructor.
     *
     * @param env    - the environment object.
     * @param dealer - the dealer object.
     * @param table  - the table object.
     * @param id     - the id of the player.
     * @param human  - true iff the player is a human player (i.e. input is provided manually, via the keyboard).
     */
    public int counter;

    public Player(Env env, Dealer dealer, Table table, int id, boolean human) {
        this.env = env;
        this.lock = new Object() ;
        this.table = table;
        this.id = id;
        this.human = human;
        this.counter=0;
        this.isPoint=false;
        this.isPenalty=false;
        this.dealerResponded = false ;
    }

    /**
     * The main player thread of each player starts here (main loop for the player thread).
     */
    @Override
    public void run() {
        playerThread = Thread.currentThread();
        env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
        if (!human) createArtificialIntelligence();
        while (!terminate) {
            // TODO implement main player loop
            if(dealerResponded) {
                if (isPoint) {
                    point();
                    dealerResponded = false;
                } else if (isPenalty) {
                    penalty();
                    dealerResponded = false;
                }
            }
        }
        if (!human)
            try { aiThread.join(); }
            catch (InterruptedException ignored) {}
        env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * Creates an additional thread for an AI (computer) player. The main loop of this thread repeatedly generates
     * key presses. If the queue of key presses is full, the thread waits until it is not full.
     */
    private void createArtificialIntelligence() {
        // note: this is a very, very smart AI (!)
        aiThread = new Thread(() -> {
            env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
            while (!terminate) {
                // TODO implement player key press simulator
                try{
                    Thread.sleep(20);
                    Random r = new Random();
                    int key = r.nextInt(env.config.tableSize);
                    keyPressed(key);
                }catch (InterruptedException ignored){

                }catch(Exception b){
                    if(aiThread!=null){
                        aiThread.interrupt();
                        try {
                            aiThread.join();
                        }catch (InterruptedException a){}
                    }
                    createArtificialIntelligence();
                }
            }
            env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
        }, "computer-" + id);
        aiThread.start();
    }

    /**
     * Called when the game should be terminated.
     */
    public void terminate() {
        // TODO implement
        this.terminate = true ;
    }

    /**
     * This method is called when a key is pressed.
     *
     * @param slot - the slot corresponding to the key pressed.
     */
    public void keyPressed(int slot) {
        // TODO implement
        if(isPoint || isPenalty|| table.placingCards) {
            return;
        }
        if(table.slotToCard[slot]==null){
            return;
        }
        boolean isFull = (table.playersTokens[this.id].size() ==3) ;
        if(isFull){
            if(table.playersTokens[this.id].contains(slot) ){
                table.removeToken(this.id,slot);
            }else {
                return;
            }
        }else{
            if(table.playersTokens[this.id].contains(slot)){
                table.removeToken(this.id,slot);
            }else{
                table.placeToken(this.id,slot);
            }
            isFull = (table.playersTokens[this.id].size() ==3) ;
            if(isFull){
                synchronized (table.reached3Players) {
                    table.reached3Players.add(this.id);
                }
            }
        }
    }

    /**
     * Award a point to a player and perform other related actions.
     *
     * @post - the player's score is increased by 1.
     * @post - the player's score is updated in the ui.
     */
    public void point() {
        // TODO implement
        this.score ++ ;
        int ignored = table.countCards(); // this part is just for demonstration in the unit tests
        env.ui.setScore(id, score);
        long freezeTime = env.config.pointFreezeMillis ;
        while (freezeTime > 0){
            env.ui.setFreeze(this.id , freezeTime);
            freezeTime-=1000 ;
            try {
                Thread.sleep(1000);
            }
            catch (InterruptedException e){}
        }
        env.ui.setFreeze(this.id , 0);
        isPoint =false;
    }

    /**
     * Penalize a player and perform other related actions.
     */
    public void penalty() {
        // TODO implement
        long freezeTime = env.config.penaltyFreezeMillis ;
        while (freezeTime > 0){
            env.ui.setFreeze(this.id , freezeTime);
            freezeTime-=1000 ;
            try {
                Thread.sleep(1000);
            }
            catch (InterruptedException e){}
        }
        env.ui.setFreeze(this.id , 0);
        isPenalty =false;
    }

    public int score() {
        return score;
    }
}