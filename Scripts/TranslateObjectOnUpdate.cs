using KeepCryingEngine;

public class TranslateObjectOnUpdate : Component
{
    public int movimiento;
    public float movimientoSexyFloat;
    public bool patataNarapñples;
    public string charMamon = "Blanc";

    public GameObject juegoInutil;

    public override void Update()
    {
        //Debug.Log(movimiento.ToString());
        gameObject.transform.Translate(1,0,0);
    }
}