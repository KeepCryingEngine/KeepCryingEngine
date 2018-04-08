using KeepCryingEngine;

public class TranslateObjectOnUpdate : Component
{
    public int movimiento;
    public float movimientoSexyFloat;
    public bool patataNipples;
    public string charMamon = "Blanc";

    public GameObject juegoInutil;

    public override void Update()
    {
        gameObject.GetTransform().Translate(movimientoSexyFloat * Time.GetDeltaTime(), 0,0);
    }
}