<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcuwindspddatainfo */

$this->title = 'Create Hcuwindspddatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcuwindspddatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuwindspddatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
